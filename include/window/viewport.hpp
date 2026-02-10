
#pragma once

#include "window.hpp"

#include "imgui.h"

#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "menu/fileformat.hpp"

#include <string>
#include <vector>
#include <filesystem>

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

            virtual void ready() noexcept override final;
            virtual void display() noexcept override final;

            void new_canvas(uint32_t width, uint32_t height) noexcept;
            void save_canvas(const std::filesystem::path &directory, const std::filesystem::path &file, FileFormat format) noexcept;
            bool is_empty() const noexcept;

            Canvas &get_canvas() noexcept;
        
        private:
            void _on_main_color_changed(const Color &clr) noexcept;
            void _on_second_color_changed(const Color &clr) noexcept;
    };
}
