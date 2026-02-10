
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/viewport.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "cursor.hpp"

#include "app.hpp"

#include "canvas/command/brush_command.hpp"

#include "menu/fileformat.hpp"

#include <filesystem>

#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
// #include <SDL3/SDL.h>

namespace chroma {

    ViewportWindow::ViewportWindow() noexcept
    : Window(
        "Viewport",
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    )
    {
        cmd = std::make_unique<BrushCommand>(); // default command
        // SDL_GPUDevice *device = App::get_device();

        // SDL_GPUTransferBufferCreateInfo transfer_info = {};
        // transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        // transfer_info.size = sizeof(float) * 24; // mat4 and 2 vec4

        // transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

        // SDL_GPUBufferCreateInfo uniform_info = {};
        // uniform_info.usage = SDL_GPU_BUFFERUSAGE_UNIFORM;
        // uniform_info.size = sizeof(float) * 24; // mat4 and 2 vec
    }

    void ViewportWindow::ready() noexcept
    {
        App::get_instance()->connect_signal("create_canvas_requested", this, &ViewportWindow::new_canvas);
        App::get_instance()->connect_signal("save_canvas_requested", this, &ViewportWindow::save_canvas);

        App::get_instance()->connect_signal("main_color_changed", this, &ViewportWindow::_on_main_color_changed);
        App::get_instance()->connect_signal("second_color_changed", this, &ViewportWindow::_on_second_color_changed);

        App::get_instance()->connect_signal("edit_fliph", this, &ViewportWindow::fliph);
        App::get_instance()->connect_signal("edit_flipv", this, &ViewportWindow::flipv);
    }

    void ViewportWindow::display() noexcept
    {
        ImGui::Begin(label.c_str(), nullptr, flags);

        ImGuiIO &io = ImGui::GetIO();

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = w->DrawList;
        ImDrawList* foreground = ImGui::GetForegroundDrawList();

        const ImVec2 pad = ImGui::GetStyle().WindowPadding;

        ImVec2 window_size;
        ImVec2 origin;

        const ImVec2 mouse = io.MousePos;

        SDL_GPUDevice *device = App::get_device();
        SDL_GPUCommandBuffer *cmd_buffer = App::get_command_buffer();

        // if (cmd == nullptr) {
        //     cmd = std::make_unique<BrushCommand>();
        // }

        // const ColorPickerWindow &color_pick = App::get_instance()->color_picker;

        // cmd->set_main_color(color_pick.main_color);
        // cmd->set_second_color(color_pick.second_color);

        uint64_t modal = 0;
        if (ImGui::BeginTabBar("##ViewportTabs",
            ImGuiTabBarFlags_NoCloseWithMiddleMouseButton |
            ImGuiTabBarFlags_NoTooltip |
            ImGuiTabBarFlags_Reorderable
        )) {
            window_size = ImGui::GetContentRegionAvail() + pad * 2.0f;
            origin = w->DC.CursorPos - pad;

            for (uint64_t i = 0; i < canvases.size(); ++i) {
                const Canvas &canvas = canvases[i];

                const ImVec2 canvas_size = ImVec2(canvas.width, canvas.height) * canvas.zoom;
                const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;

                ImGui::PushID(i);

                ImGuiTabItemFlags flags = 0;

                if (canvas.dirty) {
                    flags |= ImGuiTabItemFlags_UnsavedDocument;
                }

                bool open = true;
                if (ImGui::BeginTabItem(canvas.name.c_str(), &open, flags)) {
                    selected = i;
                    draw_list->PushClipRectFullScreen();
                    draw_list->AddRectFilled(origin, origin + window_size, IM_COL32(101, 85, 97, 255));

                    draw_list->PopClipRect();

                    draw_list->PushClipRect(
                        origin,
                        origin + window_size
                    );

                    draw_list->AddRectFilled(
                        canvas_offset - ImVec2(2, 2),
                        canvas_offset + canvas_size + ImVec2(2, 2),
                        IM_COL32(0, 0, 0, 255)
                    );

                    ImGui::RenderColorRectWithAlphaCheckerboard(
                        draw_list,
                        canvas_offset,
                        canvas_offset + canvas_size,
                        0,
                        16.0f * canvas.zoom,
                        ImVec2(0, 0)
                    );

                    for (uint64_t i = 0; i < canvas.layers.size(); i++) {
                        draw_list->AddImage(
                            (ImTextureRef)(uintptr_t)canvas.layers[i].texture,
                            canvas_offset,
                            canvas_offset + canvas_size
                        );

                        if (i == canvas.layer) {
                            draw_list->AddImage(
                                (ImTextureRef)(uintptr_t)canvas.preview,
                                canvas_offset,
                                canvas_offset + canvas_size
                            );
                        }
                    }

                    draw_list->PopClipRect();

                    ImGui::EndTabItem();
                }

                ImGui::PopID();

                if (!open) {
                    modal = i;
                    const Canvas &canvas = canvases[i];
                    if (canvas.dirty) {
                        ImGui::PushOverrideID(64);
                        ImGui::OpenPopup("Warning");
                        ImGui::PopID();
                    } else {
                        canvases.erase(canvases.begin() + i);
                        --i;
                    }
                }
            }

            ImGui::EndTabBar();
        }

        dragging = ImGui::IsMouseDown(ImGuiMouseButton_Middle);

        ImGui::PushOverrideID(64);

        if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Save changes to '%s' before closing?\n\n", canvases[selected].name.c_str());

            if (ImGui::BeginTable("##ModalButtons", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody)) {
                ImGui::TableNextColumn();
                if (ImGui::Button("Save", ImVec2(-FLT_MIN, 0))) {
                    // Save file
                }
                ImGui::SetItemDefaultFocus();

                ImGui::TableNextColumn();
                if (ImGui::Button("Discard", ImVec2(-FLT_MIN, 0))) {
                    canvases.erase(canvases.begin() + selected);
                    if (selected == canvases.size() && selected > 0) {
                        selected--;
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::TableNextColumn();
                if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndTable();
            }

            ImGui::EndPopup();
        }

        ImGui::PopID();

        if (canvases.empty()) {
            ImGui::End();
            return;
        }

        Canvas &canvas = canvases[selected];
        Color old;

        const ImVec2 canvas_size = ImVec2(canvas.width, canvas.height) * canvas.zoom;
        const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;

        if (ImGui::IsMouseHoveringRect(canvas_offset, canvas_offset + canvas_size)) {
            const ImVec2 local = mouse - canvas_offset;\
            const ImVec2 local_zoomed = local * (1.0f / canvas.zoom);
            // const ImVec2 local = local_zoomed * (1.0f / canvas.zoom);
            const ImVec2 snapped = ImVec2(
                floorf(local_zoomed.x),
                floorf(local_zoomed.y)
            );

            uint32_t x = static_cast<uint32_t>(snapped.x);
            uint32_t y = static_cast<uint32_t>(snapped.y);

            old = canvas.get_color(x, y);

            // printf("Mouse at (%f, %f) -> Local (%f, %f) -> Snapped (%f, %f) -> Pos (%u, %u)\n",
            //     mouse.x, mouse.y,
            //     local.x, local.y,
            //     snapped.x, snapped.y,
            //     x, y
            // );

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !discarded) {
                if (!brushing) {
                    cmd->start(x, y, old);
                    brushing = true;
                } else {
                    cmd->update(x, y, old);
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                        discarded = true;
                        cmd->discard();
                        brushing = false;
                    }
                }
            } else if (brushing) {
                cmd->end(x, y, old);
                canvas.add_command(std::move(cmd));
                brushing = false;

                canvas.dirty = true;

                // Prepare new command
                cmd = std::make_unique<BrushCommand>();
            } else {
                discarded = ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right);
            }
        }
        
        

        if (ImGui::IsMouseHoveringRect(origin, origin + window_size)) {
            // if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
            //     ImGui::OpenPopup("MyPopup");
            // if (ImGui::BeginPopup("MyPopup"))
            // {
            //     ImGui::Text("Hello popup !");
            //     void *pixels = canvas.layers[0].data;
            //     // void* pixels = SDL_MapGPUTransferBuffer(device, canvas.layers[0].buffer, false);
            //     SDL_Surface* surface = SDL_CreateSurfaceFrom(16, 16, SDL_PIXELFORMAT_RGBA32, pixels, 16 * 4);
            //     // if (SDL_FlipSurface(surface, SDL_FLIP_VERTICAL) == true);
            //     //     ImGui::Text("FLIPED SURFACE");
            //     if (SDL_SaveBMP(surface, "./chroma.bmp") == true)
            //         ImGui::Text("BMP SAVED");
                
            //     if (ImGui::Button("cool :)")) {
            //         ImGui::CloseCurrentPopup();
            //     }
            //     SDL_DestroySurface(surface);
            //     // SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

            //     ImGui::EndPopup();
            // }

            CursorManager::set_cursor(Cursor::Cross);

            dragging = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
            
            if (io.MouseWheel > 0.0f) {
                canvas.zoom = std::min(canvas.zoom * 1.5f, 16.0f);
            } else if (io.MouseWheel < 0.0f) {
                canvas.zoom = std::max(canvas.zoom * 0.75f, 0.1f);
            }
        }

        if (dragging) {
            CursorManager::set_cursor(Cursor::Grab);
            ImVec2 mouse_delta = io.MouseDelta;
            canvas.offset += mouse_delta;
            
            // const ImVec2 canvas_end = canvas_offset + canvas_size;
            // const ImVec2 window_end = origin + window_size;

            // if (canvas_offset.x < origin.x) {
            //     canvas.offset.x += (origin.x - canvas_offset.x);
            // }
            // if (canvas_offset.y < origin.y) {
            //     canvas.offset.y += (origin.y - canvas_offset.y);
            // }

            // if (canvas_end.x > window_end.x) {
            //     canvas.offset.x -= canvas_end.x - window_end.x;
            // }
            // if (canvas_end.y > window_end.y) {
            //     canvas.offset.y -= canvas_end.y - window_end.y;
            // }
        }

        ImGui::End();

        constexpr float near = 0.0f;
        constexpr float far = 10.0f;

        float data[24] = {0.0f};
        // float ortho[4][4] = (float*)data;
        data[0] = 2.0f / canvas.width;
        data[5] = -2.0f / canvas.height;
        data[10] = -2.0f / (far - near);
        data[12] = -1.0f;
        data[13] = 1.0f;
        data[14] = -(far + near) / (far - near);
        data[15] = 1.0f;

        cmd->get_main_color().upload(&data[16]);
        cmd->get_second_color().upload(&data[20]);

        SDL_PushGPUVertexUniformData(
            cmd_buffer,
            0,
            data,
            sizeof(data)
        );

        SDL_GPUColorTargetInfo target_info = {};
        target_info.texture = canvas.preview;
        target_info.clear_color = SDL_FColor { 1.0f, 1.0f, 1.0f, 0.0f };
        target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        target_info.store_op = SDL_GPU_STOREOP_STORE;
        target_info.mip_level = 0;
        target_info.layer_or_depth_plane = 0;
        target_info.cycle = false;

        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(
            cmd_buffer,
            &target_info,
            1,
            nullptr
        );

        cmd->preview(render_pass);

        SDL_EndGPURenderPass(render_pass);

        if (!canvas.pending.empty()) {
            canvas.execute_pending();
        }

        SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd_buffer);

        canvas.upload(copy_pass);

        SDL_EndGPUCopyPass(copy_pass);
    }

    void ViewportWindow::new_canvas(uint32_t width, uint32_t height) noexcept
    {
        canvases.emplace_back(width, height);

        // if (!canvas.layers[0].texture) {
        //     return false;
        // }

        // if (!canvas.layers[0].buffer) {
        //     return false;
        // }

        // if (!canvas.preview) {
        //     return false;
        // }

        // return true;
    }

    void ViewportWindow::save_canvas(
        const std::filesystem::path &directory,
        const std::filesystem::path &file,
        FileFormat format
    ) noexcept
    {
        Canvas &canvas = canvases[selected];

        SDL_GPUDevice *device = App::get_device();

        std::filesystem::path file_path = directory / file;
        const char *path = file_path.c_str();

        canvas.name = file;
        canvas.dirty = false;

        void *pixels = SDL_MapGPUTransferBuffer(device, canvas.layers[0].buffer, true);
        SDL_Surface* surface = SDL_CreateSurfaceFrom(canvas.width, canvas.height, SDL_PIXELFORMAT_RGBA32, pixels, canvas.width * 4);

        bool result = false;

        switch (format) {
            case BMP: {
                result = IMG_SaveBMP(surface, path);
            } break;
            case JPG: {
                result = IMG_SaveJPG(surface, path, 100);
            } break;
            case PNG: {
                result = IMG_SavePNG(surface, path);
            } break;
            case TGA: {
                result = IMG_SaveTGA(surface, path);
            } break;
        }

        SDL_DestroySurface(surface);
        SDL_UnmapGPUTransferBuffer(device, canvas.layers[0].buffer);
    }

    void ViewportWindow::fliph() {
        Canvas &canvas = canvases[selected];
        const Layer &layer = canvas.layers[canvas.layer];
        
        SDL_GPUDevice *device = App::get_device();

        void *pixels = SDL_MapGPUTransferBuffer(device, layer.buffer, true);
        SDL_Surface* surface = SDL_CreateSurfaceFrom(canvas.width, canvas.height, SDL_PIXELFORMAT_RGBA32, pixels, canvas.width * 4);

        SDL_FlipSurface(surface, SDL_FlipMode::SDL_FLIP_HORIZONTAL);

        SDL_DestroySurface(surface);
        SDL_UnmapGPUTransferBuffer(device, layer.buffer);

            canvas.refresh();
    }    

    void ViewportWindow::flipv() {
        Canvas &canvas = canvases[selected];
        const Layer &layer = canvas.layers[canvas.layer];
        
        SDL_GPUDevice *device = App::get_device();

        void *pixels = SDL_MapGPUTransferBuffer(device, layer.buffer, true);
        SDL_Surface* surface = SDL_CreateSurfaceFrom(canvas.width, canvas.height, SDL_PIXELFORMAT_RGBA32, pixels, canvas.width * 4);

        SDL_FlipSurface(surface, SDL_FlipMode::SDL_FLIP_VERTICAL);

        SDL_DestroySurface(surface);
        SDL_UnmapGPUTransferBuffer(device, layer.buffer);

            canvas.refresh();
    }

    bool ViewportWindow::is_empty() const noexcept
    {
        return canvases.empty();
    }

    Canvas &ViewportWindow::get_canvas() noexcept
    {
        return canvases[selected];
    }

    void ViewportWindow::_on_main_color_changed(const Color &clr) noexcept
    {
        cmd->set_main_color(clr);
    }

    void ViewportWindow::_on_second_color_changed(const Color &clr) noexcept
    {
        cmd->set_second_color(clr);
    }
    
}
