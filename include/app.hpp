
#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include "color.hpp"

#include "window/window.hpp"
#include "window/color_picker.hpp"
#include "window/viewport.hpp"
#include "window/palette.hpp"

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

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

            template <typename T>
            T *get_window(const std::string &label) const noexcept {
                if (!windows.contains(label)) return nullptr;
                return (T*)windows.at(label).get();
            }

            static App* get_instance() noexcept;
            static SDL_GPUDevice* get_device() noexcept;

            // ViewportWindow viewport_window;
            // ColorPickerWindow color_picker;
            // PaletteWindow palette_window;
        
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
            SDL_GPUDevice* device = nullptr;

            bool dockspace_initialized = false;
            bool done = false;
            bool idle = false;

            lua_State *state;

            std::unordered_map<std::string, std::unique_ptr<Window>> windows;

            static App* instance;
    };
}
