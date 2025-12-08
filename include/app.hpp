
#pragma once

#include "SDL3/SDL.h"
#include "color.hpp"

namespace chroma {

    class App {
        public:
            App() = default;
            ~App();

            App(const App&) = delete;
            App& operator=(const App&) = delete;

            App(App&&) = delete;
            App& operator=(App&&) = delete;
            
            int init();
            int run();
        
        private:
            int create_window();
            int create_device();

            int setup();
            int setup_imgui();
            int setup_imgui_dockspace();
            int imgui_dockspace();

            int process_events();

            Color main_color;
            Color second_color;

            SDL_Window* window = nullptr;
            SDL_GPUDevice* device = nullptr;

            bool dockspace_initialized = false;
            bool done = false;
            bool idle = false;
    };
}
