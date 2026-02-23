
#define IMGUI_DEFINE_MATH_OPERATORS

#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "canvas/canvas.hpp"
#include "canvas/command/select/select_move_command.hpp"

#include "app.hpp"

#include <cstring>

namespace chroma {

    SelectMoveCommand::SelectMoveCommand() noexcept {}

    SelectMoveCommand::~SelectMoveCommand() noexcept {}

    // void SelectMoveCommand::add(uint32_t x, uint32_t y,
    //                        const Color &color) noexcept {
    //     positions.push_back({(float)x, (float)y});
    //     previous_colors.push_back(color);
    // }

    // bool SelectMoveCommand::contains(uint32_t x, uint32_t y) const noexcept {
    //     for (size_t i = 0; i < positions.size(); ++i) {
    //         if (positions[i].x == x && positions[i].y == y) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    void SelectMoveCommand::redo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        // uint8_t *mapping;
        // int pitch;
        // SDL_LockTexture(texture, NULL, (void**)&mapping, &pitch);

        // for (const SDL_FPoint &p : positions) {
        //     int x = p.x;
        //     int y = p.y;

        //     MASK.upload(mapping + (x + y * canvas.width) * 4);
        // }

        // SDL_UnlockTexture(texture);
        SDL_Point delta{
            e.x - s.x,
            e.y - s.y,
        };
        App::get_instance()->emit_signal<SDL_Point>("select_move", delta);
    }

    void SelectMoveCommand::undo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        // uint8_t *mapping;
        // int pitch;
        // SDL_LockTexture(texture, NULL, (void**)&mapping, &pitch);

        // for (uint32_t i = 0; i < positions.size(); i++) {
        //     const SDL_FPoint &p = positions[i];
        //     int x = p.x;
        //     int y = p.y;

        //     previous_colors[i].upload(mapping + (x + y * canvas.width) * 4);
        // }

        // SDL_UnlockTexture(texture);
        // App::get_instance()->emit_signal<SDL_Rect>("select_mark", this->rect);
        SDL_Point delta{
            s.x - e.x,
            s.y - e.y,
        };
        App::get_instance()->emit_signal<SDL_Point>("select_move", delta);
    }

    void SelectMoveCommand::start(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        s = p;
        e = p;

        App::get_instance()->emit_signal("select_move_start");
    }

    void SelectMoveCommand::update(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        // if (e.x != p.x && e.y != p.y) {
        SDL_Point delta{
            p.x - e.x,
            p.y - e.y,
        };
        App::get_instance()->emit_signal<SDL_Point>("select_move", delta);
        e = p;
        // }
    }

    void SelectMoveCommand::end(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        update(canvas, p, color);

        App::get_instance()->emit_signal("select_move_end");

        // App::get_instance()->emit_signal<SDL_Rect>("select_mark", this->rect);
    }

    void SelectMoveCommand::discard(const Canvas &canvas) noexcept { e = s; }

    // void SelectMoveCommand::imgui_draw(ImVec2 pos, ImVec2 size) noexcept {

    // }

    void SelectMoveCommand::preview(const Canvas &canvas) noexcept {
        // SDL_Renderer *renderer = App::get_renderer();

        // SDL_SetRenderTarget(renderer, canvas.overlay);
        // SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        // SDL_RenderClear(renderer);

        // if (this->rect.w == 0) {
        //     // SDL_SetRenderTarget(renderer, NULL);
        //     return;
        // }

        // SDL_SetRenderTarget(renderer, canvas.overlay);

        // SDL_FRect rect = {this->rect.x, this->rect.y, this->rect.w,
        //                   this->rect.h};

        // SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 1.f);
        // SDL_RenderRect(renderer, &rect);
        // SDL_SetRenderTarget(renderer, NULL);

        // const Layer &layer = canvas.layers[canvas.layer];
        // SDL_Renderer *renderer = App::get_renderer();

        // SDL_SetRenderTarget(renderer, canvas.preview);
        // SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        // SDL_RenderClear(renderer);
        // SDL_SetRenderTarget(renderer, NULL);

        // if (positions.empty()) {
        //     return;
        // }

        // uint8_t *mapping;
        // int pitch;
        // SDL_LockTexture(layer.texture, NULL, (void**)&mapping, &pitch);

        // for (const SDL_FPoint &p : positions) {
        //     int x = p.x;
        //     int y = p.y;

        //     MASK.upload(mapping + (x + y * canvas.width) * 4);
        // }

        // SDL_UnlockTexture(layer.texture);
    }

} // namespace chroma
