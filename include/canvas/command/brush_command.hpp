
#pragma once

#include "color_command.hpp"

#include <vector>

namespace chroma {

    class BrushCommand : public ColorCommand {
        struct Pos {
            uint32_t x;
            uint32_t y;
        };

        std::vector<Pos> positions;
        std::vector<Color> previous_colors;

        public:
            BrushCommand() noexcept = default;

            void add(uint32_t x, uint32_t y, const Color &old) noexcept;
            bool contains(uint32_t x, uint32_t y) const noexcept;

            virtual void redo(Canvas &canvas) noexcept override final;
            virtual void undo(Canvas &canvas) noexcept override final;

            virtual void start(uint32_t x, uint32_t y, const Color &color) noexcept override final;
            virtual void update(uint32_t x, uint32_t y, const Color &color) noexcept override final;
            virtual void end(uint32_t x, uint32_t y, const Color &color) noexcept override final;
    };

}
