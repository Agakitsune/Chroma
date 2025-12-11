
#pragma once

#include "window.hpp"

#include "imgui.h"

#include "SDL3/SDL.h"

#include <string>
#include <vector>

namespace chroma {

    class ViewportWindow : public Window {
        struct Canvas {
            std::string name = "Untitled";

            std::vector<SDL_Texture*> layers;
            SDL_Texture *preview;
            uint64_t layer;

            ImVec2 offset = ImVec2(0, 0);
            float zoom = 1.0f;

            bool dirty = false;
        };

        std::vector<Canvas> canvases;

        bool dragging = false;

        public:
            ViewportWindow() noexcept;

            virtual void display() noexcept override final;

            bool new_canvas(SDL_Renderer *renderer, int width, int height, const std::string &name = "") noexcept;
    };
}
