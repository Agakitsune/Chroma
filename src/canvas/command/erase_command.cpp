
#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "canvas/command/erase_command.hpp"

#include "app.hpp"

#include <cstring>
namespace chroma {

    EraseCommand::EraseCommand() noexcept {
    }

    EraseCommand::~EraseCommand() noexcept {
    }

    void EraseCommand::add(uint32_t x, uint32_t y,
                           const Color &color) noexcept {
        positions.push_back({(float)x, (float)y});
        previous_colors.push_back(color);
    }

    bool EraseCommand::contains(uint32_t x, uint32_t y) const noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].x == x && positions[i].y == y) {
                return true;
            }
        }
        return false;
    }

    void EraseCommand::redo(const Canvas &canvas) noexcept {
        const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(layer.texture, NULL, (void**)&mapping, &pitch);

        for (const SDL_FPoint &p : positions) {
            int x = p.x;
            int y = p.y;

            MASK.upload(mapping + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(layer.texture);
    }

    void EraseCommand::undo(const Canvas &canvas) noexcept {
        const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(layer.texture, NULL, (void**)&mapping, &pitch);

        for (uint32_t i = 0; i < positions.size(); i++) {
            const SDL_FPoint &p = positions[i];
            int x = p.x;
            int y = p.y;

            previous_colors[i].upload(mapping + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(layer.texture);
    }

    void EraseCommand::start(uint32_t x, uint32_t y,
                             const Color &color) noexcept {
        add(x, y, color);
    }

    void EraseCommand::update(uint32_t x, uint32_t y,
                              const Color &color) noexcept {
        if (!contains(x, y)) {
            add(x, y, color);
        }
    }

    void EraseCommand::end(uint32_t x, uint32_t y,
                           const Color &color) noexcept {
        update(x, y, color);
    }

    void EraseCommand::discard(const Canvas &preview) noexcept {
        undo(preview);
        positions.clear();
        previous_colors.clear();
    }

    void EraseCommand::preview(const Canvas &canvas) noexcept {
        const Layer &layer = canvas.layers[canvas.layer];
        SDL_Renderer *renderer = App::get_renderer();

        SDL_SetRenderTarget(renderer, canvas.preview);
        SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);

        if (positions.empty()) {
            return;
        }

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(layer.texture, NULL, (void**)&mapping, &pitch);

        for (const SDL_FPoint &p : positions) {
            int x = p.x;
            int y = p.y;

            MASK.upload(mapping + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(layer.texture);
    }

} // namespace chroma
