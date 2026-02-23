
#include "canvas/cmd/select_mark_command.hpp"
#include "canvas/cmd/select_move_command.hpp"
#include "canvas/canvas.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

#include <cstring>

namespace chroma {

    void SelectMoveCommand::redo(SDL_Rect &selection) noexcept {
        // selection = rect;
    }

    void SelectMoveCommand::undo(SDL_Rect &selection) noexcept {
        // selection = previous;
    }

    void SelectMoveCommand::start(const SDL_Point &point, SDL_Rect &selection) noexcept {
        // off.x = selection.x;
        // off.y = selection.y;
        s = point;

        const uint64_t stride = selection.w * 4;
        const uint64_t buffer_size = selection.h * stride;
        const uint64_t skip = (selection.x + selection.y * surface->w) * 4;

        SDL_Renderer *renderer = App::get_renderer();
        uint8_t *mapping = (uint8_t *)surface->pixels;

        data = new uint8_t[buffer_size];
        previous = new uint8_t[buffer_size];

        mapping += skip;

        for (uint32_t i = 0; i < selection.h; i++) {
            std::memcpy(&data[i * stride], mapping, stride);
            std::memset(mapping, 0, stride);
            mapping += surface->pitch;
        }

        tex = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            selection.w,
            selection.h
        );

        // uint8_t *mapping;
        int pitch;
        SDL_LockTexture(texture, &selection, (void **)&mapping, &pitch);

        std::memset(mapping, 0, pitch * selection.h);

        SDL_UnlockTexture(texture);

        SDL_UpdateTexture(tex, nullptr, data, stride);
    }

    void SelectMoveCommand::update(const SDL_Point &point, SDL_Rect &selection) noexcept {
        e = point;
    }

    void SelectMoveCommand::end(const SDL_Point &point, SDL_Rect &selection) noexcept {
        update(point, selection);
    }

    void SelectMoveCommand::discard(SDL_Rect &selection) noexcept {
        e = s;
    }

    void SelectMoveCommand::render(SDL_Texture *preview, SDL_Texture *overlay, SDL_Rect &selection) noexcept {
        SDL_Renderer *renderer = App::get_renderer();

        SDL_SetRenderTarget(renderer, preview);

        SDL_FPoint delta{
            e.x - s.x,
            e.y - s.y
        };

        SDL_FRect rect = {
            selection.x + delta.x,
            selection.y + delta.y,
            selection.w,
            selection.h
        };
        
        SDL_RenderTexture(renderer, tex, nullptr, &rect);
        SDL_SetRenderTarget(renderer, NULL);
    }

    std::unique_ptr<MouseCommand> SelectMoveCommand::next() noexcept {
        // // Overwrite the data
        // const uint64_t stride = tex->w * 4;
        // const uint64_t buffer_size = tex->h * stride;
        // const uint64_t skip = (selection.x + selection.y * surface->w) * 4;

        // SDL_Renderer *renderer = App::get_renderer();
        // uint8_t *mapping = (uint8_t *)surface->pixels;

        // data = new uint8_t[buffer_size];
        // previous = new uint8_t[buffer_size];

        // mapping += skip;

        // for (uint32_t i = 0; i < selection.h; i++) {
        //     std::memcpy(&data[i * stride], mapping, stride);
        //     std::memset(mapping, 0, stride);
        //     mapping += surface->pitch;
        // }

        return std::make_unique<SelectMarkCommand>();
    }

    std::unique_ptr<MouseCommand> SelectMoveCommand::transform() noexcept {
        return std::unique_ptr<MouseCommand>(this);
    }

}
