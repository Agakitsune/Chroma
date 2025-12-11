
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/viewport.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "cursor.hpp"

#include "app.hpp"

#include "SDL3/SDL_render.h"

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

        SDL_Renderer *renderer = App::get_renderer();

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

                const ImVec2 canvas_size = ImVec2(canvas.preview->w, canvas.preview->h) * canvas.zoom;
                const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;

                ImGui::PushID(i);

                ImGuiTabItemFlags flags = 0;

                if (canvas.dirty) {
                    flags |= ImGuiTabItemFlags_UnsavedDocument;
                }

                bool open = true;
                if (ImGui::BeginTabItem(canvas.name.c_str(), &open, flags)) {
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
                            (ImTextureRef)(uintptr_t)canvas.layers[i],
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
                        SDL_DestroyTexture(canvases[modal].preview);

                        for (SDL_Texture* layer : canvases[modal].layers) {
                            SDL_DestroyTexture(layer);
                        }
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

            SDL_SetRenderTarget(renderer, canvas.preview);

            SDL_SetRenderDrawColorFloat(renderer, 0.0f, 0.0f, 0.0f, 0.0f);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColorFloat(renderer, 0.0f, 0.0f, 0.0f, 1.0f);

            const ImVec2 canvas_size = ImVec2(canvas.preview->w, canvas.preview->h) * canvas.zoom;

            const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;

            if (ImGui::IsMouseHoveringRect(canvas_offset, canvas_offset + canvas_size)) {
                const ImVec2 local = mouse - canvas_offset;
                const ImVec2 snapped = ImVec2(
                    floorf(local.x / canvas.preview->w),
                    floorf(local.y / canvas.preview->h)
                );
                
                SDL_RenderPoint(renderer, snapped.x, snapped.y);

                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    canvas.dirty = true;

                    SDL_SetRenderTarget(renderer, canvas.layers[canvas.layer]);

                    SDL_RenderTexture(renderer, canvas.preview, nullptr, nullptr);
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
                    // Discard changes
                    if (canvases[modal].preview) {
                        SDL_DestroyTexture(canvases[modal].preview);

                        for (SDL_Texture* layer : canvases[modal].layers) {
                            SDL_DestroyTexture(layer);
                        }
                    }
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

    bool ViewportWindow::new_canvas(SDL_Renderer *renderer, int width, int height, const std::string &name) noexcept
    {
        Canvas &canvas = canvases.emplace_back();

        canvas.layers.push_back(SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        ));
        canvas.layer = 0;

        canvas.preview = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        );

        if (!canvas.layers[0]) {
            return false;
        }

        if (!canvas.preview) {
            return false;
        }

        SDL_SetTextureScaleMode(canvas.layers[0], SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
        SDL_SetTextureScaleMode(canvas.preview, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

        canvas.name = name.empty() ? "Untitled" : name;
        // canvas.dirty = true;

        return true;
    }

}
