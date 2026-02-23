
#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "canvas/command/brush_command.hpp"

#include "app.hpp"

#include <cstring>

namespace chroma {
    BrushCommand::BrushCommand() noexcept {}

    BrushCommand::~BrushCommand() noexcept {}

    void BrushCommand::add(const SDL_Point &point,
                           const Color &color) noexcept {
        positions.push_back({(float)point.x, (float)point.y});
        previous_colors.push_back(color);
    }

    bool BrushCommand::contains(const SDL_Point &point) const noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].x == point.x && positions[i].y == point.y) {
                return true;
            }
        }
        return false;
    }

    void BrushCommand::redo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (const SDL_FPoint &p : positions) {
            int x = p.x;
            int y = p.y;

            main.upload(mapping + (x + y * canvas.width) * 4);
            main.upload(((uint8_t *)surface->pixels) +
                        (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void BrushCommand::undo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (uint32_t i = 0; i < positions.size(); i++) {
            const SDL_FPoint &p = positions[i];
            int x = p.x;
            int y = p.y;

            previous_colors[i].upload(mapping + (x + y * canvas.width) * 4);
            previous_colors[i].upload(((uint8_t *)surface->pixels) +
                                      (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void BrushCommand::start(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        add(p, color);
    }

    void BrushCommand::update(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        if (!contains(p)) {
            add(p, color);
        }
    }

    void BrushCommand::end(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        update(canvas, p, color);
    }

    void BrushCommand::discard(const Canvas &canvas) noexcept {
        positions.clear();
        previous_colors.clear();
    }

    void BrushCommand::preview(const Canvas &canvas) noexcept {
        SDL_Renderer *renderer = App::get_renderer();

        // SDL_SetRenderTarget(renderer, canvas.preview);
        // SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        // SDL_RenderClear(renderer);

        if (positions.empty()) {
            // SDL_SetRenderTarget(renderer, NULL);
            return;
        }

        // SDL_SetRenderTarget(renderer, canvas.preview);

        // SDL_SetRenderDrawColorFloat(renderer, main.r, main.g, main.b, main.a);
        // SDL_RenderPoints(renderer, positions.data(), positions.size());

        // SDL_SetRenderTarget(renderer, NULL);
    }

} // namespace chroma
