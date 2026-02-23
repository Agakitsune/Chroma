
#include "canvas/cmd/mark_command.hpp"
#include "canvas/canvas.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

namespace chroma {

    void MarkCommand::redo(SDL_Rect &selection) noexcept {
        selection = rect;
    }

    void MarkCommand::undo(SDL_Rect &selection) noexcept {
        selection = previous;
    }

}
