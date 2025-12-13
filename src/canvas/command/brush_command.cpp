
#include "canvas/command/brush_command.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"

#include "canvas/command/brush_command.hpp"
#include "canvas/canvas.hpp"

namespace chroma {

    void BrushCommand::add(uint32_t x, uint32_t y, const Color &color) noexcept {
        printf("%p Adding position (%u, %u) with previous color (%f, %f, %f, %f)\n",
            this,
            x, y,
            color.r, color.g, color.b, color.a
        );
        positions.push_back({x, y});
        previous_colors.push_back(color);
    }

    bool BrushCommand::contains(uint32_t x, uint32_t y) const noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].x == x && positions[i].y == y) {
                return true;
            }
        }
        return false;
    }

    void BrushCommand::redo(Canvas &canvas) noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            const Pos &pos = positions[i];
            canvas.set_color(pos.x, pos.y, main);
        }
    }

    void BrushCommand::undo(Canvas &canvas) noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            const Pos &pos = positions[i];
            const Color &old_color = previous_colors[i];
            canvas.set_color(pos.x, pos.y, old_color);
        }
    }

    void BrushCommand::start(uint32_t x, uint32_t y, const Color &color) noexcept {
        add(x, y, color);
    }

    void BrushCommand::update(uint32_t x, uint32_t y, const Color &color) noexcept {
        if (!contains(x, y)) {
            add(x, y, color);
        }
    }

    void BrushCommand::end(uint32_t x, uint32_t y, const Color &color) noexcept {
        update(x, y, color);
    }

}
