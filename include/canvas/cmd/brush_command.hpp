
#pragma once

#include "mouse_command.hpp"

#include <vector>

namespace chroma {

    class BrushCommand : public MouseCommand {
        std::vector<SDL_FPoint> points;
        std::vector<Color> old;

      public:
        BrushCommand() noexcept = default;
        BrushCommand(const BrushCommand &cmd) noexcept : MouseCommand(cmd) {}
        virtual ~BrushCommand() noexcept override = default;

        virtual void redo(SDL_Rect &selection) noexcept override final;
        virtual void undo(SDL_Rect &selection) noexcept override final;

        virtual void start(const SDL_Point &point, SDL_Rect &selection) noexcept override final;
        virtual void update(const SDL_Point &point, SDL_Rect &selection) noexcept override final;
        virtual void end(const SDL_Point &point, SDL_Rect &selection) noexcept override final;
        virtual void discard(SDL_Rect &selection) noexcept override final;

        virtual void render(
            SDL_Texture *preview,
            SDL_Texture *overlay,
            SDL_Rect &selection
        ) noexcept override final;

        virtual std::unique_ptr<MouseCommand> next() noexcept override final;
    };

}
