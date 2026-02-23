
#pragma once

#include "color_command.hpp"

#include "SDL3/SDL.h"

#include <vector>

namespace chroma {

    class ShapeCommand : public ColorCommand {
        SDL_Point s;
        SDL_Point e;
        SDL_Rect rect = {0, 0, 0, 0};

        uint8_t *previous_data = nullptr;

      public:
        ShapeCommand() noexcept;
        virtual ~ShapeCommand() noexcept override;

        virtual void redo(const Canvas &canvas) noexcept override final;
        virtual void undo(const Canvas &canvas) noexcept override final;

        virtual void start(const Canvas &canvas,
                           const SDL_Point &point,
                           const Color &color) noexcept override final;
        virtual void update(const Canvas &canvas,
                           const SDL_Point &point,
                           const Color &color) noexcept override final;
        virtual void end(const Canvas &canvas,
                           const SDL_Point &point,
                           const Color &color) noexcept override final;
        virtual void discard(const Canvas &canvas) noexcept override final;

        virtual void preview(const Canvas &canvas) noexcept override final;
    };

} // namespace chroma
