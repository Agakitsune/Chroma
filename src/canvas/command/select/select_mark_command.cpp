
#define IMGUI_DEFINE_MATH_OPERATORS

#include "SDL3/SDL.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "canvas/canvas.hpp"
#include "canvas/command/select/select_mark_command.hpp"

#include "app.hpp"

#include <cstring>

namespace chroma {

    SelectMarkCommand::SelectMarkCommand() noexcept {}

    SelectMarkCommand::~SelectMarkCommand() noexcept {}

    // void SelectMarkCommand::add(uint32_t x, uint32_t y,
    //                        const Color &color) noexcept {
    //     positions.push_back({(float)x, (float)y});
    //     previous_colors.push_back(color);
    // }

    // bool SelectMarkCommand::contains(uint32_t x, uint32_t y) const noexcept {
    //     for (size_t i = 0; i < positions.size(); ++i) {
    //         if (positions[i].x == x && positions[i].y == y) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    void SelectMarkCommand::redo(const Canvas &canvas) noexcept {
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
        App::get_instance()->emit_signal<SDL_Rect>("select_mark", this->rect);
    }

    void SelectMarkCommand::undo(const Canvas &canvas) noexcept {
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
    }

    void SelectMarkCommand::start(uint32_t x, uint32_t y,
                                  const Color &color) noexcept {
        s = SDL_Point{(int)x, (int)y};
        rect.w = 0;
    }

    void SelectMarkCommand::update(uint32_t x, uint32_t y,
                                   const Color &color) noexcept {
        e = SDL_Point{(int)x, (int)y};
        SDL_Point min = SDL_Point{std::min(s.x, e.x), std::min(s.y, e.y)};
        SDL_Point max = SDL_Point{std::max(s.x, e.x), std::max(s.y, e.y)};
        this->rect =
            SDL_Rect{min.x, min.y, max.x - min.x + 1, max.y - min.y + 1};
    }

    void SelectMarkCommand::end(uint32_t x, uint32_t y,
                                const Color &color) noexcept {
        update(x, y, color);

        App::get_instance()->emit_signal<SDL_Rect>("select_mark", this->rect);
    }

    void SelectMarkCommand::discard() noexcept { rect.w = 0; }

    // void SelectMarkCommand::imgui_draw(ImVec2 pos, ImVec2 size) noexcept {

    // }

    void SelectMarkCommand::preview(const Canvas &canvas) noexcept {
        SDL_Renderer *renderer = App::get_renderer();

        SDL_SetRenderTarget(renderer, canvas.overlay);
        SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        SDL_RenderClear(renderer);

        if (this->rect.w == 0) {
            SDL_SetRenderTarget(renderer, NULL);
            return;
        }

        SDL_FRect rect = {this->rect.x, this->rect.y, this->rect.w,
                          this->rect.h};

        SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 1.f);
        SDL_RenderRect(renderer, &rect);
        SDL_SetRenderTarget(renderer, NULL);

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
