
#pragma once

#include "imgui.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"

#include "command/command.hpp"

#include <string>
#include <vector>
#include <queue>
#include <memory>

namespace chroma {

    struct Layer {
        uint8_t *data = nullptr;
        bool *dirty_flags = nullptr;

        SDL_GPUTransferBuffer *buffer = nullptr;
        SDL_GPUTexture *texture = nullptr;

        Layer() noexcept = default;
        ~Layer() noexcept;

        Layer(const Layer&) = delete;
        Layer& operator=(const Layer&) = delete;

        Layer(Layer&&) noexcept;
        Layer& operator=(Layer&&) noexcept;
    };

    struct Canvas {
        std::string name = "Untitled";

        std::vector<Layer> layers;

        SDL_GPUTexture *preview = nullptr;

        std::vector<std::unique_ptr<ICommand>> stack;
        std::queue<std::unique_ptr<ICommand>> pending;
        
        uint32_t stack_index = 0;

        uint32_t layer = 0;

        uint32_t width = 0;
        uint32_t height = 0;

        // -- ImGui related --

        ImVec2 offset = ImVec2(0, 0);
        float zoom = 1.0f;

        bool dirty = false;

        Canvas(uint32_t width, uint32_t height) noexcept;
        ~Canvas() noexcept;

        Canvas(const Canvas&) = delete;
        Canvas& operator=(const Canvas&) = delete;

        Canvas(Canvas&&) noexcept;
        Canvas& operator=(Canvas&&) noexcept;

        Color get_color(uint32_t x, uint32_t y) const noexcept;
        void set_color(uint32_t x, uint32_t y, const Color &color) noexcept;

        void add_command(std::unique_ptr<ICommand> &&cmd) noexcept;
        void execute_pending() noexcept;

        void undo() noexcept;
        void redo() noexcept;

        void upload(SDL_GPUCopyPass *pass) noexcept;

        private:
            struct TileTransfer {
                uint32_t index;

                uint32_t w;
                uint32_t h;
                
                uint32_t x;
                uint32_t y;
            };

            std::vector<TileTransfer> pending_uploads;
    };

}
