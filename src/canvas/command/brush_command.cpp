
#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "canvas/command/brush_command.hpp"

#include "app.hpp"

#include <cstring>

namespace chroma {
    BrushCommand::BrushCommand() noexcept {
    }

    BrushCommand::~BrushCommand() noexcept {
    }

    void BrushCommand::add(uint32_t x, uint32_t y,
                           const Color &color) noexcept {
        positions.push_back({(float)x, (float)y});
        previous_colors.push_back(color);
    }

    bool BrushCommand::contains(uint32_t x, uint32_t y) const noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].x == x && positions[i].y == y) {
                return true;
            }
        }
        return false;
    }

    void BrushCommand::redo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void**)&mapping, &pitch);

        for (const SDL_FPoint &p : positions) {
            int x = p.x;
            int y = p.y;

            main.upload(mapping + (x + y * canvas.width) * 4);
            main.upload(((uint8_t*)surface->pixels) + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void BrushCommand::undo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void**)&mapping, &pitch);

        for (uint32_t i = 0; i < positions.size(); i++) {
            const SDL_FPoint &p = positions[i];
            int x = p.x;
            int y = p.y;

            previous_colors[i].upload(mapping + (x + y * canvas.width) * 4);
            previous_colors[i].upload(((uint8_t*)surface->pixels) + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void BrushCommand::start(uint32_t x, uint32_t y,
                             const Color &color) noexcept {
        add(x, y, color);
    }

    void BrushCommand::update(uint32_t x, uint32_t y,
                              const Color &color) noexcept {
        if (!contains(x, y)) {
            add(x, y, color);
        }
    }

    void BrushCommand::end(uint32_t x, uint32_t y,
                           const Color &color) noexcept {
        update(x, y, color);
    }

    void BrushCommand::discard() noexcept {
        positions.clear();
        previous_colors.clear();
    }

    void BrushCommand::preview(const Canvas &canvas) noexcept {
        SDL_Renderer *renderer = App::get_renderer();

        SDL_SetRenderTarget(renderer, canvas.preview);
        SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        SDL_RenderClear(renderer);

        if (positions.empty()) {
            SDL_SetRenderTarget(renderer, NULL);
            return;
        }
        
        SDL_SetRenderDrawColorFloat(renderer, main.r, main.g, main.b, main.a);
        SDL_RenderPoints(renderer, positions.data(), positions.size());
        
        SDL_SetRenderTarget(renderer, NULL);
    }

} // namespace chroma
