
#pragma once

#include "color_command.hpp"

#include "SDL3/SDL.h"

#include <vector>

namespace chroma {

    class EraseCommand : public ICommand {
        std::vector<SDL_FPoint> positions; // forced to use floats since SDL doesn't have a method to draw with int :c
        std::vector<Color> previous_colors;

      public:
        EraseCommand() noexcept;
        virtual ~EraseCommand() noexcept override;

        void add(uint32_t x, uint32_t y, const Color &old) noexcept;
        bool contains(uint32_t x, uint32_t y) const noexcept;

        virtual void redo(const Canvas &canvas) noexcept override final;
        virtual void undo(const Canvas &canvas) noexcept override final;

        virtual void start(uint32_t x, uint32_t y,
                           const Color &color) noexcept override final;
        virtual void update(uint32_t x, uint32_t y,
                            const Color &color) noexcept override final;
        virtual void end(uint32_t x, uint32_t y,
                         const Color &color) noexcept override final;
        virtual void discard() noexcept override final;

        virtual void preview(const Canvas &canvas) noexcept override final;
    };

} // namespace chroma
