
#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "canvas/command/erase_command.hpp"

#include "app.hpp"

#include <cstring>
namespace chroma {

    EraseCommand::EraseCommand() noexcept {}

    EraseCommand::~EraseCommand() noexcept {}

    void EraseCommand::add(const SDL_Point &point,
                           const Color &color) noexcept {
        positions.push_back({(float)point.x, (float)point.y});
        previous_colors.push_back(color);
    }

    bool EraseCommand::contains(const SDL_Point &point) const noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].x == point.x && positions[i].y == point.y) {
                return true;
            }
        }
        return false;
    }

    void EraseCommand::redo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (const SDL_FPoint &p : positions) {
            int x = p.x;
            int y = p.y;

            MASK.upload(mapping + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void EraseCommand::undo(const Canvas &canvas) noexcept {
        // const Layer &layer = canvas.layers[canvas.layer];

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (uint32_t i = 0; i < positions.size(); i++) {
            const SDL_FPoint &p = positions[i];
            int x = p.x;
            int y = p.y;

            previous_colors[i].upload(mapping + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void EraseCommand::start(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        add(p, color);
    }

    void EraseCommand::update(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        if (!contains(p)) {
            add(p, color);
        }
    }

    void EraseCommand::end(const Canvas &canvas, const SDL_Point &p, const Color &color) noexcept {
        update(canvas, p, color);
    }

    void EraseCommand::discard(const Canvas &canvas) noexcept {
        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (uint32_t i = 0; i < positions.size(); i++) {
            const SDL_FPoint &p = positions[i];
            int x = p.x;
            int y = p.y;

            previous_colors[i].upload(mapping + x * 4 + y * surface->pitch);
        }

        SDL_UnlockTexture(texture);

        positions.clear();
        previous_colors.clear();
    }

    void EraseCommand::preview(const Canvas &canvas) noexcept {
        const Layer &layer = canvas.layers[canvas.layer];
        // SDL_Renderer *renderer = App::get_renderer();

        // SDL_SetRenderTarget(renderer, canvas.preview);
        // SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
        // SDL_RenderClear(renderer);
        // SDL_SetRenderTarget(renderer, NULL);

        if (positions.empty()) {
            return;
        }

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(layer.texture, NULL, (void **)&mapping, &pitch);

        for (const SDL_FPoint &p : positions) {
            int x = p.x;
            int y = p.y;

            MASK.upload(mapping + (x + y * canvas.width) * 4);
        }

        SDL_UnlockTexture(layer.texture);
    }

} // namespace chroma
