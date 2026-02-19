
#pragma once

#include "SDL3/SDL.h"

#include "color.hpp"

namespace chroma {

    struct Canvas;

    class ICommand {
      public:
        virtual ~ICommand() noexcept = default;

        virtual void redo(const Canvas &canvas) noexcept = 0;
        virtual void undo(const Canvas &canvas) noexcept = 0;

        virtual void set_main_color(const Color &color) noexcept {};
        virtual void set_second_color(const Color &color) noexcept {};

        virtual Color get_main_color() noexcept { return MASK; }

        virtual Color get_second_color() noexcept { return MASK; }

        virtual void start(uint32_t x, uint32_t y,
                           const Color &color) noexcept = 0;
        virtual void update(uint32_t x, uint32_t y,
                            const Color &color) noexcept = 0;
        virtual void end(uint32_t x, uint32_t y,
                         const Color &color) noexcept = 0;
        virtual void discard(const Canvas &preview) noexcept = 0;

        virtual void preview(const Canvas &preview) noexcept = 0;
    };

} // namespace chroma
