/**
 * @file color_command.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "canvas/command/command.hpp"

#include "color.hpp"

namespace chroma {

    struct Canvas;

    class SelectMarkCommand : public ICommand {
        SDL_Point s;
        SDL_Point e;
        SDL_Rect rect = {0, 0, 0, 0};

        // uint8_t *previous_data = nullptr;
        // std::vector<SDL_FPoint> positions; // forced to use floats since SDL
        // doesn't have a method to draw with int :c std::vector<Color>
        // previous_colors;

      public:
        SelectMarkCommand() noexcept;
        virtual ~SelectMarkCommand() noexcept override;

        // void add(uint32_t x, uint32_t y, const Color &old) noexcept;
        // bool contains(uint32_t x, uint32_t y) const noexcept;

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
