/**
 * @file brush_command.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "color_command.hpp"

#include "SDL3/SDL.h"

#include <vector>

namespace chroma {

    class BrushCommand : public ColorCommand {
        std::vector<SDL_FPoint> positions; // forced to use floats since SDL doesn't have a method to draw with int :c
        std::vector<Color> previous_colors;

      public:
        BrushCommand() noexcept;
        virtual ~BrushCommand() noexcept override;

        void add(uint32_t x, uint32_t y, const Color &old) noexcept;
        bool contains(uint32_t x, uint32_t y) const noexcept;

        virtual void redo(const Canvas &canvas) noexcept override final;
        virtual void undo(const Canvas &canvas) noexcept override final;

        // virtual void start(uint32_t x, uint32_t y,
        //                    const Color &color) noexcept override final;
        // virtual void update(uint32_t x, uint32_t y,
        //                     const Color &color) noexcept override final;
        // virtual void end(uint32_t x, uint32_t y,
        //                  const Color &color) noexcept override final;
        // virtual void discard(const Canvas &preview) noexcept override final;
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
