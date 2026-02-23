
#include "canvas/cmd/flip_command.hpp"
#include "canvas/canvas.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

namespace chroma {

    void FlipCommand::redo(SDL_Rect &selection) noexcept {
        uint8_t *mapping = (uint8_t *)surface->pixels;

        if (selection.w > 0) {
            SDL_Surface *tmp = SDL_CreateSurface(
                selection.w,
                selection.h,
                surface->format
            );

            SDL_Rect dst = {
                0, 0, selection.w, selection.h
            };

            SDL_BlitSurfaceUnchecked(surface, &selection, tmp, &dst);
            SDL_FlipSurface(tmp, mode);
            SDL_FillSurfaceRect(surface, &selection, MASK.to_u32());
            SDL_BlitSurfaceUnchecked(tmp, &dst, surface, &selection);

            mapping += selection.x * 4 + selection.y * surface->pitch;
            SDL_UpdateTexture(texture, &selection, mapping, surface->pitch);
        } else {
            SDL_FlipSurface(surface, mode);
            SDL_UpdateTexture(texture, nullptr, surface->pixels, surface->pitch);
        }
    }

    void FlipCommand::undo(SDL_Rect &selection) noexcept {
        redo(selection); // :)
    }

}
