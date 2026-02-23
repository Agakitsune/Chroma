/**
 * @file command.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "command.hpp"

// #include "SDL3/SDL.h"

#include "color.hpp"

#include <memory>

namespace chroma {

    // struct Canvas;
    // struct Layer;

    class MouseCommand : public Command {
      protected:
        // Color data
        Color main;
        Color second;

      public:
        MouseCommand() noexcept = default;
        MouseCommand(const MouseCommand &cmd) noexcept : main(cmd.main), second(cmd.second) {}
        virtual ~MouseCommand() noexcept override = default;

        virtual void start(const SDL_Point &point, SDL_Rect &selection) noexcept = 0;
        virtual void update(const SDL_Point &point, SDL_Rect &selection) noexcept = 0;
        virtual void end(const SDL_Point &point, SDL_Rect &selection) noexcept = 0;
        virtual void discard(SDL_Rect &selection) noexcept = 0;

        virtual void render(
            SDL_Texture *preview,
            SDL_Texture *overlay,
            SDL_Rect &selection
        ) noexcept = 0;

        // virtual void redo(SDL_Rect *selection) noexcept = 0;
        // virtual void undo(SDL_Rect *selection) noexcept = 0;

        // virtual void set_layer(const Layer &layer) noexcept;
        // virtual ~ICommand() noexcept = default;

        // virtual void redo(const Canvas &canvas) noexcept = 0;
        // virtual void undo(const Canvas &canvas) noexcept = 0;

        void set_main_color(const Color &color) noexcept;
        void set_second_color(const Color &color) noexcept;

        virtual bool is_persitent() noexcept { return false; }
        virtual bool can_transform() noexcept { return false; }

        virtual std::unique_ptr<MouseCommand> next() noexcept = 0;
        virtual std::unique_ptr<MouseCommand> transform() noexcept { nullptr; }

        // virtual void set_layer(const Layer &layer) noexcept;

        // virtual Color get_main_color() noexcept { return BLACK; }
        // virtual Color get_second_color() noexcept { return BLACK; }

        // virtual void start(const Canvas &canvas,
        //                    const SDL_Point &point,
        //                    const Color &color) noexcept = 0;
        // virtual void update(const Canvas &canvas,
        //                    const SDL_Point &point,
        //                    const Color &color) noexcept = 0;
        // virtual void end(const Canvas &canvas,
        //                    const SDL_Point &point,
        //                    const Color &color) noexcept = 0;
        // virtual void discard(const Canvas &canvas) noexcept = 0;

        // virtual void preview(const Canvas &canvas) noexcept = 0;
    };

} // namespace chroma
