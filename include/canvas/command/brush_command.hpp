
#pragma once

#include "color_command.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"

#include <vector>

namespace chroma {

    class BrushCommand : public ColorCommand {
        struct Pos {
            float x;
            float y;
        };

        std::vector<Pos> positions;
        std::vector<Color> previous_colors;

        SDL_GPUGraphicsPipeline *pipeline = nullptr;
        SDL_GPUShader *vertex_shader = nullptr;
        SDL_GPUShader *fragment_shader = nullptr;

        SDL_GPUBuffer *instance_buffer = nullptr;

        public:
            BrushCommand() noexcept;
            ~BrushCommand() noexcept;

            void add(uint32_t x, uint32_t y, const Color &old) noexcept;
            bool contains(uint32_t x, uint32_t y) const noexcept;

            virtual void redo(Canvas &canvas) noexcept override final;
            virtual void undo(Canvas &canvas) noexcept override final;

            virtual void start(uint32_t x, uint32_t y, const Color &color) noexcept override final;
            virtual void update(uint32_t x, uint32_t y, const Color &color) noexcept override final;
            virtual void end(uint32_t x, uint32_t y, const Color &color) noexcept override final;
            virtual void discard() noexcept override final;

            virtual void preview(SDL_GPURenderPass *render_pass) const noexcept override final;
    };

}
