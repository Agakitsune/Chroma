/**
 * @file viewport.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Handling windows displayed (canva, colors, colorpicker, etc)
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/viewport.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "cursor.hpp"

#include "app.hpp"

#include "canvas/cmd/brush_command.hpp"
#include "canvas/cmd/erase_command.hpp"
#include "canvas/cmd/flip_command.hpp"
#include "canvas/cmd/mark_command.hpp"
#include "canvas/cmd/select_mark_command.hpp"
#include "canvas/cmd/shape_command.hpp"

#include "menu/fileformat.hpp"

#include <cstring>
#include <filesystem>

#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

namespace chroma {

ViewportWindow::ViewportWindow() noexcept
    : Window("Viewport",
          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)
{
    cmd = std::make_unique<BrushCommand>(); // default command
}

void ViewportWindow::ready() noexcept
{
    App::get_instance()->connect_signal("create_canvas_requested", this,
        &ViewportWindow::new_canvas);
    App::get_instance()->connect_signal("save_canvas_requested", this,
        &ViewportWindow::save_canvas);
    App::get_instance()->connect_signal("open_canvas_requested", this,
        &ViewportWindow::open_canvas);

    App::get_instance()->connect_signal("main_color_changed", this,
        &ViewportWindow::_on_main_color_changed);
    App::get_instance()->connect_signal(
        "second_color_changed", this, &ViewportWindow::_on_second_color_changed);

    App::get_instance()->connect_signal("edit_fliph", this,
        &ViewportWindow::fliph);
    App::get_instance()->connect_signal("edit_flipv", this,
        &ViewportWindow::flipv);

    App::get_instance()->connect_signal("edit_undo", this, &ViewportWindow::undo);
    App::get_instance()->connect_signal("edit_redo", this, &ViewportWindow::redo);

    App::get_instance()->connect_signal("layer_new", this,
        &ViewportWindow::add_layer);
    App::get_instance()->connect_signal("layer_delete", this,
        &ViewportWindow::delete_layer);

    App::get_instance()->connect_signal("select_all", this,
        &ViewportWindow::select_all);
}

void ViewportWindow::display() noexcept
{
    ImGui::Begin(label.c_str(), nullptr, flags);

    ImGuiIO& io = ImGui::GetIO();

    ImGuiWindow* w = ImGui::GetCurrentWindow();
    ImDrawList* draw_list = w->DrawList;
    ImDrawList* foreground = ImGui::GetForegroundDrawList();

    const ImVec2 pad = ImGui::GetStyle().WindowPadding;

    ImVec2 window_size;
    ImVec2 origin;

    const ImVec2 mouse = io.MousePos;

    SDL_Renderer* renderer = App::get_renderer();

    if (marked < canvases.size()) {
        canvases.erase(canvases.begin() + marked);
        if (selected == canvases.size() && selected > 0) {
            selected--;
        }
        marked = canvases.size();
    }

    if (ImGui::IsKeyChordPressed(ImGuiKey_B)) {
        cmd = std::make_unique<BrushCommand>(*cmd); // default command
    } else if (ImGui::IsKeyChordPressed(ImGuiKey_S)) {
        cmd = std::make_unique<ShapeCommand>(*cmd);
    } else if (ImGui::IsKeyChordPressed(ImGuiKey_E)) {
        cmd = std::make_unique<EraseCommand>(*cmd);
    } else if (ImGui::IsKeyChordPressed(ImGuiKey_M)) {
        cmd = std::make_unique<SelectMarkCommand>(*cmd);
    }

    uint64_t modal = 0;
    if (ImGui::BeginTabBar("##ViewportTabs",
            ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_Reorderable)) {
        window_size = ImGui::GetContentRegionAvail() + pad * 2.0f;
        origin = w->DC.CursorPos - pad;

        for (uint64_t i = 0; i < canvases.size(); ++i) {
            const Canvas& canvas = canvases[i];

            const ImVec2 canvas_size = ImVec2(canvas.width, canvas.height) * canvas.zoom;
            const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;

            ImGui::PushID(i);

            ImGuiTabItemFlags flags = 0;

            if (canvas.dirty) {
                flags |= ImGuiTabItemFlags_UnsavedDocument;
            }

            bool open = true;
            if (ImGui::BeginTabItem(canvas.name.c_str(), &open, flags)) {
                if (selected != i) {
                    selected = i;
                    App::get_instance()->emit_signal<Canvas*>("canvas_selected",
                        &canvases[selected]);
                    reload();
                }

                draw_list->PushClipRectFullScreen();
                draw_list->AddRectFilled(origin, origin + window_size,
                    IM_COL32(101, 85, 97, 255));

                draw_list->PopClipRect();

                draw_list->PushClipRect(origin, origin + window_size);

                draw_list->AddRectFilled(canvas_offset - ImVec2(2, 2),
                    canvas_offset + canvas_size + ImVec2(2, 2),
                    IM_COL32(0, 0, 0, 255));

                ImGui::RenderColorRectWithAlphaCheckerboard(
                    draw_list, canvas_offset, canvas_offset + canvas_size, 0,
                    16.0f * canvas.zoom, ImVec2(0, 0));

                for (uint64_t i = 0; i < canvas.layers.size(); i++) {
                    if (canvas.layers[i].visible) {
                        draw_list->AddImage(
                            (ImTextureRef)(uintptr_t)canvas.layers[i].texture,
                            canvas_offset, canvas_offset + canvas_size);
                    }

                    if (i == canvas.layer) {
                        draw_list->AddImage((ImTextureRef)(uintptr_t)preview, canvas_offset,
                            canvas_offset + canvas_size);
                    }
                }

                draw_list->AddImage((ImTextureRef)(uintptr_t)overlay, canvas_offset,
                    canvas_offset + canvas_size);

                draw_list->PopClipRect();

                ImGui::EndTabItem();
            }

            ImGui::PopID();

            if (!open) {
                modal = i;
                const Canvas& canvas = canvases[i];
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
                ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody)) {
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

    Canvas& canvas = canvases[selected];
    // Color old;

    cmd->set_layer(canvas.layers[canvas.layer]);

    const ImVec2 canvas_size = ImVec2(canvas.width, canvas.height) * canvas.zoom;
    const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;

    if (ImGui::IsMouseHoveringRect(canvas_offset, canvas_offset + canvas_size)) {
        const ImVec2 local = mouse - canvas_offset;
        const ImVec2 local_zoomed = local * (1.0f / canvas.zoom);
        // const ImVec2 local = local_zoomed * (1.0f / canvas.zoom);
        const ImVec2 snapped = ImVec2(floorf(local_zoomed.x), floorf(local_zoomed.y));

        int32_t x = static_cast<int32_t>(snapped.x);
        int32_t y = static_cast<int32_t>(snapped.y);

        // old = canvas.get_color(x, y);

        SDL_Point p = { x, y };

        on_select = (selection.w > 0) && SDL_PointInRect(&p, &selection);

        // printf("Mouse at (%f, %f) -> Local (%f, %f) -> Snapped (%f, %f)
        // -> Pos
        // (%u, %u)\n",
        //     mouse.x, mouse.y,
        //     local.x, local.y,
        //     snapped.x, snapped.y,
        //     x, y
        // );

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !discarded) {
            if (!brushing) {

                // if (cmd->can_transform()) {
                //     if (on_select) {
                //         cmd = cmd->transform();
                //     } else {
                //         std::unique_ptr<MouseCommand> tmp = cmd->next();

                //         if (tmp != cmd) {
                //             canvas.add_command(std::move(cmd));
                //         }

                //         // Prepare new command
                //         cmd = std::move(tmp);
                //     }
                // }

                cmd->start(p, selection);
                brushing = true;
            } else {
                cmd->update(p, selection);
                if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                    discarded = true;
                    cmd->discard(selection);
                    brushing = false;
                }
            }
        } else if (brushing) {
            cmd->end(p, selection);

            // if (!cmd->is_persitent()) {
            std::unique_ptr<MouseCommand> tmp = cmd->next();

            if (tmp != cmd) {
                canvas.add_command(std::move(cmd));
            }

            // Prepare new command
            cmd = std::move(tmp);
            // }

            brushing = false;
            canvas.dirty = true;
        } else {
            discarded = ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right);
        }
    }

    SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
    SDL_SetRenderTarget(renderer, preview);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, overlay);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);

    cmd->render(preview, overlay, selection);

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

    if (selection.w > 0) {
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = w->DrawList;
        ImDrawList* foreground = ImGui::GetForegroundDrawList();

        const ImVec2 pad = ImGui::GetStyle().WindowPadding;

        const ImVec2 window_size = ImGui::GetContentRegionAvail() + pad * 2.0f;
        const ImVec2 origin = w->DC.CursorPos - pad;

        const ImVec2 canvas_size = ImVec2(canvas.width, canvas.height) * canvas.zoom;
        const ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f + canvas.offset;
        const ImVec2 pixel_size = canvas_size / ImVec2(canvas.width, canvas.height);

        const ImVec2 start = ImVec2(selection.x * pixel_size.x, selection.y * pixel_size.y) + canvas_offset;

        const ImVec2 end = ImVec2(selection.w * pixel_size.x, selection.h * pixel_size.y) + start;

        draw_list->AddRect(start - ImVec2(2, 2), end + ImVec2(2, 2),
            IM_COL32(255, 255, 255, 255), 0.0, 0, 2.0);
    }

    ImGui::End();

    ImGui::PushOverrideID(57);
    bool c = true;
    if (ImGui::BeginPopupModal("SaveFailure", &c)) {
        ImGui::Text("Failed to save canvas: %s", SDL_GetError());
        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (!canvas.pending.empty()) {
        canvas.execute_pending(selection);
    }
}

void ViewportWindow::new_canvas(uint32_t width, uint32_t height) noexcept
{
    selected = canvases.size();
    canvases.emplace_back(width, height);
    App::get_instance()->emit_signal<Canvas*>("canvas_selected",
        &canvases[selected]);
    marked = canvases.size();

    reload();
}

void ViewportWindow::save_canvas(const std::filesystem::path& directory,
    const std::filesystem::path& file,
    FileFormat format) noexcept
{
    Canvas& canvas = canvases[selected];

    std::filesystem::path file_path = directory / file;
    const char* path = file_path.c_str();

    canvas.name = file;
    canvas.dirty = false;

    bool result = false;

    SDL_Surface* surface = SDL_CreateSurface(canvas.width, canvas.height, SDL_PIXELFORMAT_RGBA32);

    for (const auto& layer : canvas.layers) {
        if (!SDL_BlitSurface(layer.surface, nullptr, surface, nullptr)) {
            ImGui::PushOverrideID(57);
            ImGui::OpenPopup("SaveFailure");
            ImGui::PopID();
        }
    }

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

    if (!result) {
        ImGui::PushOverrideID(57);
        ImGui::OpenPopup("SaveFailure");
        ImGui::PopID();
    }

    SDL_DestroySurface(surface);
}

void ViewportWindow::open_canvas(const std::filesystem::path& directory,
    const std::filesystem::path& file,
    FileFormat format) noexcept
{
    std::filesystem::path file_path = directory / file;
    const char* path = file_path.c_str();

    SDL_IOStream* stream = SDL_IOFromFile(path, "r");
    SDL_Surface* surface = nullptr;

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

    SDL_Surface* output = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);

    selected = canvases.size();
    Canvas& canvas = canvases.emplace_back(output);
    App::get_instance()->emit_signal<Canvas*>("canvas_selected",
        &canvases[selected]);

    marked = canvases.size();

    canvas.name = file;

    reload();

    SDL_DestroySurface(surface);
    SDL_CloseIO(stream);
}

void ViewportWindow::fliph() noexcept
{
    if (canvases.empty()) {
        return;
    }

    Canvas& canvas = canvases[selected];
    const Layer& layer = canvas.layers[canvas.layer];

    std::unique_ptr<FlipCommand> cmd = std::make_unique<FlipCommand>();

    cmd->mode = SDL_FLIP_HORIZONTAL;
    cmd->set_layer(layer);

    canvas.add_command(std::move(cmd));

    //   SDL_FlipSurface(layer.surface, SDL_FlipMode::SDL_FLIP_HORIZONTAL);
    //   SDL_UpdateTexture(layer.texture, nullptr, layer.surface->pixels,
    //                     layer.surface->pitch);
}

void ViewportWindow::flipv() noexcept
{
    if (canvases.empty()) {
        return;
    }

    Canvas& canvas = canvases[selected];
    const Layer& layer = canvas.layers[canvas.layer];

    std::unique_ptr<FlipCommand> cmd = std::make_unique<FlipCommand>();

    cmd->mode = SDL_FLIP_VERTICAL;
    cmd->set_layer(layer);

    canvas.add_command(std::move(cmd));
}

void ViewportWindow::add_layer() noexcept
{
    if (canvases.empty()) {
        return;
    }

    Canvas& canvas = canvases[selected];

    canvas.add_layer();
}

void ViewportWindow::delete_layer() noexcept
{
    if (canvases.empty()) {
        return;
    }

    Canvas& canvas = canvases[selected];
    canvas.delete_layer();
}

void ViewportWindow::select_all() noexcept
{
    if (canvases.empty()) {
        return;
    }

    Canvas& canvas = canvases[selected];
    std::unique_ptr<MarkCommand> cmd = std::make_unique<MarkCommand>();

    cmd->previous = selection;

    cmd->rect.x = 0;
    cmd->rect.y = 0;
    cmd->rect.w = canvas.width;
    cmd->rect.h = canvas.height;

    canvas.add_command(std::move(cmd));
}

void ViewportWindow::undo() noexcept
{
    Canvas& canvas = canvases[selected];
    canvas.undo(selection);
}

void ViewportWindow::redo() noexcept
{
    Canvas& canvas = canvases[selected];
    canvas.redo(selection);
}

void ViewportWindow::reload() noexcept
{
    const Canvas& canvas = canvases[selected];

    SDL_Renderer* renderer = App::get_renderer();

    if (preview) {
        SDL_DestroyTexture(preview);
        SDL_DestroyTexture(overlay);
    }

    preview = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET, canvas.width, canvas.height);

    overlay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET, canvas.width, canvas.height);

    SDL_SetTextureScaleMode(preview, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(overlay, SDL_SCALEMODE_NEAREST);
}

bool ViewportWindow::is_empty() const noexcept { return canvases.empty(); }

Canvas& ViewportWindow::get_canvas() noexcept { return canvases[selected]; }

void ViewportWindow::_on_main_color_changed(const Color& clr) noexcept
{
    cmd->set_main_color(clr);
}

void ViewportWindow::_on_second_color_changed(const Color& clr) noexcept
{
    cmd->set_second_color(clr);
}

} // namespace chroma
