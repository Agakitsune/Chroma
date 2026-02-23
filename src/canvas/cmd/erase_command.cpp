
#include "canvas/cmd/erase_command.hpp"
#include "canvas/canvas.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

namespace chroma {

    void EraseCommand::redo(SDL_Rect &selection) noexcept {
        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (const SDL_Point &p : points) {

            MASK.upload(mapping + (p.x + p.y * surface->w) * 4);
            MASK.upload(((uint8_t *)surface->pixels) +
                                      (p.x + p.y * surface->w) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void EraseCommand::undo(SDL_Rect &selection) noexcept {
        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (uint32_t i = 0; i < points.size(); i++) {
            const SDL_Point &p = points[i];

            old[i].upload(mapping + (p.x + p.y * surface->w) * 4);
            old[i].upload(((uint8_t *)surface->pixels) +
                                      (p.x + p.y * surface->w) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    void EraseCommand::start(const SDL_Point &point, SDL_Rect &selection) noexcept {
        Color c;
        uint8_t *pix = (uint8_t*)surface->pixels;

        if (selection.w > 0 && !SDL_PointInRect(&point, &selection)) {
            return;
        }
        
        points.push_back(point);

        c.download(&pix[point.x * 4 + point.y * surface->pitch]);

        old.push_back(c);
    }

    void EraseCommand::update(const SDL_Point &point, SDL_Rect &selection) noexcept {
        Color c;
        uint8_t *pix = (uint8_t*)surface->pixels;

        if (selection.w > 0 && !SDL_PointInRect(&point, &selection)) {
            return;
        }

        for (const SDL_Point &pt : points) {
            if (pt.x == point.x && pt.y == point.y) {
                return;
            }
        }
        
        points.push_back(point);

        c.download(&pix[point.x * 4 + point.y * surface->pitch]);

        old.push_back(c);
    }

    void EraseCommand::end(const SDL_Point &point, SDL_Rect &selection) noexcept {
        update(point, selection);
    }

    void EraseCommand::discard(SDL_Rect &selection) noexcept {
        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (uint32_t i = 0; i < points.size(); i++) {
            const SDL_Point &p = points[i];

            old[i].upload(mapping + (p.x + p.y * surface->w) * 4);
            old[i].upload(((uint8_t *)surface->pixels) +
                                      (p.x + p.y * surface->w) * 4);
        }

        SDL_UnlockTexture(texture);

        points.clear();
        old.clear();
    }

    void EraseCommand::render(SDL_Texture *preview, SDL_Texture *overlay, SDL_Rect &selection) noexcept {
        SDL_Renderer *renderer = App::get_renderer();

        if (points.empty()) {
            return;
        }

        uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&mapping, &pitch);

        for (const SDL_Point &p : points) {

            MASK.upload(mapping + (p.x + p.y * surface->w) * 4);
        }

        SDL_UnlockTexture(texture);
    }

    std::unique_ptr<MouseCommand> EraseCommand::next() noexcept {
        return std::make_unique<EraseCommand>(*this);
    }

}
