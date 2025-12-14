
#pragma once

#include "SDL3/SDL.h"

#include "color.hpp"

namespace chroma {

    struct Canvas;

    class ICommand {
        public:
            virtual void redo(Canvas &canvas) noexcept = 0;
            virtual void undo(Canvas &canvas) noexcept = 0;

            virtual void set_main_color(const Color &color) noexcept = 0;
            virtual void set_second_color(const Color &color) noexcept = 0;

            virtual void start(uint32_t x, uint32_t y, const Color &color) noexcept = 0;
            virtual void update(uint32_t x, uint32_t y, const Color &color) noexcept = 0;
            virtual void end(uint32_t x, uint32_t y, const Color &color) noexcept = 0;
            virtual void discard() noexcept = 0;

            virtual void preview(SDL_GPURenderPass *render_pass) const noexcept = 0;
    };

}
