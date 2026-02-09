
#pragma once

#include "window.hpp"

#include "imgui.h"

#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"

#include <string>
#include <vector>

namespace chroma {

    class ViewportWindow : public Window {
        std::vector<Canvas> canvases;
        uint32_t selected = 0;

        bool dragging = false;
        bool brushing = false;
        bool discarded = false;

        std::unique_ptr<ICommand> cmd;

        SDL_GPUTransferBuffer *transfer_buffer = nullptr;
        SDL_GPUBuffer *uniform_buffer = nullptr;

        public:
            ViewportWindow() noexcept;
            virtual ~ViewportWindow() noexcept = default;

            virtual void display() noexcept override final;

            bool new_canvas(uint32_t width, uint32_t height) noexcept;
            bool save_canvas(const char *label, const char *extension) noexcept;
            bool is_empty() const noexcept;
            bool FlipLayerBuffer(SDL_GPUDevice* device, int width, int height);

            Canvas &get_canvas() noexcept;
    };
}
