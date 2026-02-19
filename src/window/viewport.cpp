
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/viewport.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "cursor.hpp"

#include "app.hpp"

#include "canvas/command/brush_command.hpp"
#include "canvas/command/shape_command.hpp"
#include "canvas/command/erase_command.hpp"

#include "menu/fileformat.hpp"

#include <cstring>
#include <filesystem>

#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
// #include <SDL3/SDL.h>

namespace chroma {

    ViewportWindow::ViewportWindow() noexcept
        : Window("Viewport",
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove) {
        cmd = std::make_unique<BrushCommand>(); // default command
    }

    void ViewportWindow::ready() noexcept {
        App::get_instance()->connect_signal("create_canvas_requested", this,
                                            &ViewportWindow::new_canvas);
        App::get_instance()->connect_signal("save_canvas_requested", this,
                                            &ViewportWindow::save_canvas);
        App::get_instance()->connect_signal("open_canvas_requested", this,
                                            &ViewportWindow::open_canvas);

        App::get_instance()->connect_signal(
            "main_color_changed", this,
            &ViewportWindow::_on_main_color_changed);
        App::get_instance()->connect_signal(
            "second_color_changed", this,
            &ViewportWindow::_on_second_color_changed);

        App::get_instance()->connect_signal("edit_fliph", this,
                                            &ViewportWindow::fliph);
        App::get_instance()->connect_signal("edit_flipv", this,
                                            &ViewportWindow::flipv);

        App::get_instance()->connect_signal("edit_undo", this,
                                            &ViewportWindow::undo);
        App::get_instance()->connect_signal("edit_redo", this,
                                            &ViewportWindow::redo);
    }

    void ViewportWindow::display() noexcept {
        ImGui::Begin(label.c_str(), nullptr, flags);

        ImGuiIO &io = ImGui::GetIO();

        ImGuiWindow *w = ImGui::GetCurrentWindow();
        ImDrawList *draw_list = w->DrawList;
        ImDrawList *foreground = ImGui::GetForegroundDrawList();

        const ImVec2 pad = ImGui::GetStyle().WindowPadding;

        ImVec2 window_size;
        ImVec2 origin;

        const ImVec2 mouse = io.MousePos;

        SDL_GPUDevice *device = App::get_device();
        SDL_GPUCommandBuffer *cmd_buffer = App::get_command_buffer();

        if (marked < canvases.size()) {
            canvases.erase(canvases.begin() + marked);
            if (selected == canvases.size() && selected > 0) {
                selected--;
            }
            marked = canvases.size();
        }

        uint64_t modal = 0;
        if (ImGui::BeginTabBar("##ViewportTabs",
                               ImGuiTabBarFlags_NoCloseWithMiddleMouseButton |
                                   ImGuiTabBarFlags_NoTooltip |
                                   ImGuiTabBarFlags_Reorderable)) {
            window_size = ImGui::GetContentRegionAvail() + pad * 2.0f;
            origin = w->DC.CursorPos - pad;

            for (uint64_t i = 0; i < canvases.size(); ++i) {
                const Canvas &canvas = canvases[i];

                const ImVec2 canvas_size =
                    ImVec2(canvas.width, canvas.height) * canvas.zoom;
                const ImVec2 canvas_offset =
                    origin + (window_size - canvas_size) * 0.5f + canvas.offset;

                ImGui::PushID(i);

                ImGuiTabItemFlags flags = 0;

                if (canvas.dirty) {
                    flags |= ImGuiTabItemFlags_UnsavedDocument;
                }

                bool open = true;
                if (ImGui::BeginTabItem(canvas.name.c_str(), &open, flags)) {
                    selected = i;
                    draw_list->PushClipRectFullScreen();
                    draw_list->AddRectFilled(origin, origin + window_size,
                                             IM_COL32(101, 85, 97, 255));

                    draw_list->PopClipRect();

                    draw_list->PushClipRect(origin, origin + window_size);

                    draw_list->AddRectFilled(canvas_offset - ImVec2(2, 2),
                                             canvas_offset + canvas_size +
                                                 ImVec2(2, 2),
                                             IM_COL32(0, 0, 0, 255));

                    ImGui::RenderColorRectWithAlphaCheckerboard(
                        draw_list, canvas_offset, canvas_offset + canvas_size,
                        0, 16.0f * canvas.zoom, ImVec2(0, 0));

                    for (uint64_t i = 0; i < canvas.layers.size(); i++) {
                        draw_list->AddImage(
                            (ImTextureRef)(uintptr_t)canvas.layers[i].texture,
                            canvas_offset, canvas_offset + canvas_size);

                        if (i == canvas.layer) {
                            draw_list->AddImage(
                                (ImTextureRef)(uintptr_t)canvas.preview,
                                canvas_offset, canvas_offset + canvas_size);
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
                        marked = i;
                        --i;
                    }
                }
            }

            ImGui::EndTabBar();
        }

        dragging = ImGui::IsMouseDown(ImGuiMouseButton_Middle);

        ImGui::PushOverrideID(64);

        if (ImGui::BeginPopupModal("Warning", nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Save changes to '%s' before closing?\n\n",
                        canvases[selected].name.c_str());

            if (ImGui::BeginTable("##ModalButtons", 3,
                                  ImGuiTableFlags_SizingStretchSame |
                                      ImGuiTableFlags_NoBordersInBody)) {
                ImGui::TableNextColumn();
                if (ImGui::Button("Save", ImVec2(-FLT_MIN, 0))) {
                    ImGui::CloseCurrentPopup();
                    App::get_instance()->emit_signal("popup_save");
                }
                ImGui::SetItemDefaultFocus();

                ImGui::TableNextColumn();
                if (ImGui::Button("Discard", ImVec2(-FLT_MIN, 0))) {
                    marked = selected;
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

        const ImVec2 canvas_size =
            ImVec2(canvas.width, canvas.height) * canvas.zoom;
        const ImVec2 canvas_offset =
            origin + (window_size - canvas_size) * 0.5f + canvas.offset;

        if (ImGui::IsMouseHoveringRect(canvas_offset,
                                       canvas_offset + canvas_size)) {
            const ImVec2 local = mouse - canvas_offset;
            const ImVec2 local_zoomed = local * (1.0f / canvas.zoom);
            // const ImVec2 local = local_zoomed * (1.0f / canvas.zoom);
            const ImVec2 snapped =
                ImVec2(floorf(local_zoomed.x), floorf(local_zoomed.y));

            uint32_t x = static_cast<uint32_t>(snapped.x);
            uint32_t y = static_cast<uint32_t>(snapped.y);

            old = canvas.get_color(x, y);

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

                auto tmp = std::make_unique<EraseCommand>();

                tmp->set_main_color(cmd->get_main_color());
                tmp->set_second_color(cmd->get_second_color());

                canvas.add_command(std::move(cmd));
                brushing = false;

                canvas.dirty = true;

                // Prepare new command
                cmd = std::move(tmp);
            } else {
                discarded = ImGui::IsMouseDown(ImGuiMouseButton_Left) ||
                            ImGui::IsMouseDown(ImGuiMouseButton_Right);
            }
        }

        if (ImGui::IsMouseHoveringRect(origin, origin + window_size)) {
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
        }

        ImGui::End();

        cmd->preview(canvas);

        if (!canvas.pending.empty()) {
            canvas.execute_pending();
        }
    }

    void ViewportWindow::new_canvas(uint32_t width, uint32_t height) noexcept {
        canvases.emplace_back(width, height);
        marked = canvases.size();
    }

    void ViewportWindow::save_canvas(const std::filesystem::path &directory,
                                     const std::filesystem::path &file,
                                     FileFormat format) noexcept {
        Canvas &canvas = canvases[selected];

        SDL_GPUDevice *device = App::get_device();

        std::filesystem::path file_path = directory / file;
        const char *path = file_path.c_str();

        canvas.name = file;
        canvas.dirty = false;

        bool result = false;

        switch (format) {
            case BMP: {
                result = IMG_SaveBMP(canvas.layers[0].surface, path);
            } break;
            case JPG: {
                result = IMG_SaveJPG(canvas.layers[0].surface, path, 100);
            } break;
            case PNG: {
                result = IMG_SavePNG(canvas.layers[0].surface, path);
            } break;
            case TGA: {
                result = IMG_SaveTGA(canvas.layers[0].surface, path);
            } break;
        }
    }

    void ViewportWindow::open_canvas(const std::filesystem::path &directory,
                                     const std::filesystem::path &file,
                                     FileFormat format) noexcept {
        std::filesystem::path file_path = directory / file;
        const char *path = file_path.c_str();

        SDL_IOStream *stream = SDL_IOFromFile(path, "r");
        SDL_Surface *surface = nullptr;
        SDL_GPUDevice *device = App::get_device();

        switch (format) {
            case BMP: {
                surface = IMG_LoadBMP_IO(stream);
            } break;
            case JPG: {
                surface = IMG_LoadJPG_IO(stream);
            } break;
            case PNG: {
                surface = IMG_LoadPNG_IO(stream);
            } break;
            case TGA: {
                surface = IMG_LoadTGA_IO(stream);
            } break;
        }

        SDL_Surface *output =
            SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);

        canvases.emplace_back(output);
        canvases.back().name = file;
        marked = canvases.size();

        SDL_DestroySurface(surface);
        SDL_CloseIO(stream);
    }

    void ViewportWindow::fliph() noexcept {
        Canvas &canvas = canvases[selected];
        const Layer &layer = canvas.layers[canvas.layer];

        SDL_FlipSurface(layer.surface, SDL_FlipMode::SDL_FLIP_HORIZONTAL);
        SDL_UpdateTexture(layer.texture, nullptr, layer.surface->pixels, layer.surface->pitch);
    }

    void ViewportWindow::flipv() noexcept {
        Canvas &canvas = canvases[selected];
        const Layer &layer = canvas.layers[canvas.layer];

        SDL_FlipSurface(layer.surface, SDL_FlipMode::SDL_FLIP_VERTICAL);
        SDL_UpdateTexture(layer.texture, nullptr, layer.surface->pixels, layer.surface->pitch);
    }

    void ViewportWindow::undo() noexcept {
        Canvas &canvas = canvases[selected];
        canvas.undo();
    }

    void ViewportWindow::redo() noexcept {
        Canvas &canvas = canvases[selected];
        canvas.redo();
    }

    bool ViewportWindow::is_empty() const noexcept { return canvases.empty(); }

    Canvas &ViewportWindow::get_canvas() noexcept { return canvases[selected]; }

    void ViewportWindow::_on_main_color_changed(const Color &clr) noexcept {
        cmd->set_main_color(clr);
    }

    void ViewportWindow::_on_second_color_changed(const Color &clr) noexcept {
        cmd->set_second_color(clr);
    }

} // namespace chroma
