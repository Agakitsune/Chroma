
#include "canvas/cmd/shape_command.hpp"
#include "canvas/canvas.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

#include <cstring>

namespace chroma {

    void ShapeCommand::redo(SDL_Rect &selection) noexcept {
        const uint64_t stride = rect.w * 4;
        const uint64_t buffer_size = rect.h * stride;
        const uint64_t skip = (rect.x + rect.y * surface->w) * 4;

        // if (!previous_data) {
        //     previous_data = new uint8_t[buffer_size];
        // }
        // uint8_t *mapping = (uint8_t *)surface->pixels;

        // mapping += skip;

        // for (uint32_t i = 0; i < rect.h; i++) {
        //     std::memcpy(&previous_data[i * stride], mapping, stride);
        //     mapping += surface->pitch;
        // }

        SDL_FillSurfaceRect(surface, &this->rect, main.to_u32());
        SDL_UpdateTexture(texture, &this->rect,
                          (uint8_t *)surface->pixels + skip, surface->pitch);
    }

    void ShapeCommand::undo(SDL_Rect &selection) noexcept {
        const uint64_t stride = rect.w * 4;
        const uint64_t buffer_size = rect.h * stride;
        const uint64_t skip = (rect.x + rect.y * surface->w) * 4;

        uint8_t *mapping = (uint8_t *)surface->pixels;

        mapping += skip;

        for (uint32_t i = 0; i < rect.h; i++) {
            std::memcpy(mapping, &previous_data[i * stride], stride);
            mapping += surface->pitch;
        }

        SDL_UpdateTexture(texture, &this->rect,
                          (uint8_t *)surface->pixels + skip, surface->pitch);
    }

    void ShapeCommand::start(const SDL_Point &point, SDL_Rect &selection) noexcept {
        s = point;
        rect.w = 0;
    }

    void ShapeCommand::update(const SDL_Point &point, SDL_Rect &selection) noexcept {
        e = point;
        SDL_Point min = SDL_Point{std::min(s.x, e.x), std::min(s.y, e.y)};
        SDL_Point max = SDL_Point{std::max(s.x, e.x), std::max(s.y, e.y)};
        this->rect =
            SDL_Rect{min.x, min.y, max.x - min.x + 1, max.y - min.y + 1};
    }

    void ShapeCommand::end(const SDL_Point &point, SDL_Rect &selection) noexcept {
        update(point, selection);

        if (selection.w > 0) {
            SDL_GetRectIntersection(&rect, &selection, &rect);
        }

        const uint64_t stride = rect.w * 4;
        const uint64_t buffer_size = rect.h * stride;
        const uint64_t skip = (rect.x + rect.y * surface->w) * 4;

        if (!previous_data) {
            previous_data = new uint8_t[buffer_size];
        }
        uint8_t *mapping = (uint8_t *)surface->pixels;

        mapping += skip;

        for (uint32_t i = 0; i < rect.h; i++) {
            std::memcpy(&previous_data[i * stride], mapping, stride);
            mapping += surface->pitch;
        }
    }

    void ShapeCommand::discard(SDL_Rect &selection) noexcept {
        rect.w = 0;
    }

    void ShapeCommand::render(SDL_Texture *preview, SDL_Texture *overlay, SDL_Rect &selection) noexcept {
        SDL_Renderer *renderer = App::get_renderer();

        if (this->rect.w == 0) {
            return;
        }

        SDL_SetRenderTarget(renderer, preview);

        if (selection.w > 0) {
            SDL_SetRenderClipRect(renderer, &selection);
        }

        SDL_FRect rect = {this->rect.x, this->rect.y, this->rect.w,
                          this->rect.h};

        SDL_SetRenderDrawColorFloat(renderer, main.r, main.g, main.b, main.a);
        SDL_RenderRect(renderer, &rect);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderClipRect(renderer, NULL);
    }

    std::unique_ptr<MouseCommand> ShapeCommand::next() noexcept {
        return std::make_unique<ShapeCommand>(*this);
    }

}
