
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/viewport.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "cursor.hpp"

#include "app.hpp"

#include "canvas/command/brush_command.hpp"

namespace chroma {

    ViewportWindow::ViewportWindow() noexcept
    : Window(
        "Viewport",
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    )
    {}

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

        if (cmd == nullptr) {
            cmd = std::make_unique<BrushCommand>();
        }

        ColorPickerWindow *color_pick = App::get_instance()->get_window<ColorPickerWindow>("ColorPicker");

        cmd->set_main_color(color_pick->main_color);
        cmd->set_second_color(color_pick->second_color);

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

        if (canvases.size() > 0) {
            Canvas &canvas = canvases[modal];

            Color old;
            // Color main = App::get_instance()->get_window<ColorPickerWindow>("ColorPicker")->main_color;

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
                
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    printf("Brushing at (%u, %u)\n", x, y);
                    if (!brushing) {
                        printf("Starting brush command %p\n", cmd.get());
                        cmd->start(x, y, old);
                    } else {
                        printf("Updating brush command %p\n", cmd.get());
                        cmd->update(x, y, old);
                    }
                    brushing = true;
                    canvas.dirty = true;
                } else if (brushing) {
                    printf("Brushing at (%u, %u)\n", x, y);
                    printf("Ending brush command %p\n", cmd.get());
                    cmd->end(x, y, old);
                    canvas.add_command(std::move(cmd));
                    // canvas.execute_command(std::move(cmd));
                    brushing = false;

                    // Prepare new command
                    cmd = std::make_unique<BrushCommand>();
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
                
                const ImVec2 canvas_end = canvas_offset + canvas_size;
                const ImVec2 window_end = origin + window_size;

                if (canvas_offset.x < origin.x) {
                    canvas.offset.x += (origin.x - canvas_offset.x);
                }
                if (canvas_offset.y < origin.y) {
                    canvas.offset.y += (origin.y - canvas_offset.y);
                }

                if (canvas_end.x > window_end.x) {
                    canvas.offset.x -= canvas_end.x - window_end.x;
                }
                if (canvas_end.y > window_end.y) {
                    canvas.offset.y -= canvas_end.y - window_end.y;
                }
            }
        }

        ImGui::PushOverrideID(64);

        if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Save changes to '%s' before closing?\n\n", canvases[modal].name.c_str());

            if (ImGui::BeginTable("##ModalButtons", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody)) {
                ImGui::TableNextColumn();
                if (ImGui::Button("Save", ImVec2(-FLT_MIN, 0))) {
                    // Save file
                }
                ImGui::SetItemDefaultFocus();

                ImGui::TableNextColumn();
                if (ImGui::Button("Discard", ImVec2(-FLT_MIN, 0))) {
                    canvases.erase(canvases.begin() + modal);
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

        ImGui::End();
    }

    bool ViewportWindow::new_canvas(uint32_t width, uint32_t height) noexcept
    {
        Canvas &canvas = canvases.emplace_back(width, height);

        if (!canvas.layers[0].texture) {
            return false;
        }

        if (!canvas.layers[0].buffer) {
            return false;
        }

        if (!canvas.preview) {
            return false;
        }

        return true;
    }

    bool ViewportWindow::is_empty() const noexcept
    {
        return canvases.empty();
    }

    Canvas &ViewportWindow::get_canvas() noexcept
    {
        return canvases[selected];
    } 

}
