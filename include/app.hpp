
#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"
#include "color.hpp"

#include "window/window.hpp"
#include "window/color_picker.hpp"
#include "window/viewport.hpp"

#include "lua.hpp"

#include <unordered_map>
#include <memory>

namespace chroma {

    class App {
        public:
            App() noexcept = default;
            ~App() noexcept;

            App(const App&) = delete;
            App& operator=(const App&) = delete;

            App(App&&) = delete;
            App& operator=(App&&) = delete;
            
            int init() noexcept;
            int run() noexcept;

            // template <typename T>
            // T *get_window(const std::string &label) const noexcept {
            //     if (!windows.contains(label)) return nullptr;
            //     return (T*)windows[label].get();
            // }

            static SDL_Renderer *get_renderer() noexcept;
        
        private:
            int create_window() noexcept;
            int create_device() noexcept;

            int setup() noexcept;
            int setup_imgui() noexcept;
            // int setup_windows() noexcept;
            int setup_imgui_dockspace() noexcept;
            int imgui_dockspace() noexcept;

            int process_events(float delta) noexcept;

            uint32_t w;
            uint32_t h;

            // Color main_color;
            // Color second_color;

            SDL_Window* window = nullptr;
            SDL_Renderer *renderer = nullptr;
            SDL_Texture *canvas = nullptr;
            // SDL_GPUDevice* device = nullptr;

            bool dockspace_initialized = false;
            bool done = false;
            bool idle = false;

            lua_State *state;

            ViewportWindow viewport_window;
            ColorPickerWindow color_picker;

            std::unordered_map<std::string, Window*> windows;

            static App* instance;
    };
}
