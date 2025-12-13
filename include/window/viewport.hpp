
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

        std::unique_ptr<ICommand> cmd;

        public:
            ViewportWindow() noexcept;

            virtual void display() noexcept override final;

            bool new_canvas(uint32_t width, uint32_t height) noexcept;
            bool is_empty() const noexcept;

            Canvas &get_canvas() noexcept;
    };
}
