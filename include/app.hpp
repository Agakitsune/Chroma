
#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include "color.hpp"

#include "window/window.hpp"
#include "window/color_picker.hpp"
#include "window/viewport.hpp"
#include "window/palette.hpp"

#include "system/signal.hpp"

#include "menu/menuitem.hpp"

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
            //     return (T*)windows.at(label).get();
            // }

            template<typename... A>
            void add_signal(const std::string &name) {
                std::size_t hash = typeid(void(*)(std::decay_t<A>...)).hash_code();
                signals.insert_or_assign(name, Signal());
                signal_hash.insert_or_assign(name, hash);
            }

            template<typename O, typename... A>
            void connect_signal(const std::string &name, O *object, void (O::*func)(A...)) {
                std::size_t hash = typeid(void(*)(std::decay_t<A>...)).hash_code();
                if (!signals.contains(name)) {
                    return;
                }
                if (signal_hash[name] != hash) {
                    return;
                }
                signals[name].connect(object, func);
            }

            void disconnect_signal(const std::string &name, void *object) {
                if (!signals.contains(name)) {
                    return;
                }
                signals[name].disconnect(object);
            }

            template<typename... A>
            void emit_signal(const std::string &name, A&&... args) {
                std::size_t hash = typeid(void(*)(std::decay_t<A>...)).hash_code();
                if (!signals.contains(name)) {
                    return;
                }
                if (signal_hash[name] != hash) {
                    return;
                }
                signals[name].emit(std::forward<A>(args)...);
            }

            template<typename I>
            void add_menu(const std::string &menu)
            {
                // if (!menu_bar.contains(menu)) {
                //     menu_bar[menu]menu, std::vector<std::unique_ptr<MenuItem>>());
                // }
                menu_bar[menu].push_back(std::make_unique<I>());
            }

            void separator(const std::string &menu)
            {
                // if (!menu_bar.contains(menu)) {
                //     menu_bar.insert(menu, std::vector<std::unique_ptr<MenuItem>>());
                // }
                menu_bar[menu].emplace_back(nullptr);
            }

            static App* get_instance() noexcept;
            static SDL_GPUDevice* get_device() noexcept;
            static SDL_GPUCommandBuffer *get_command_buffer() noexcept;

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
            SDL_GPUCommandBuffer *cmd_buffer = nullptr;
            
            SDL_GPUSampler *sampler = nullptr;

            bool dockspace_initialized = false;
            bool done = false;
            bool idle = false;


            std::unordered_map<std::string, std::unique_ptr<Window>> windows;
            std::unordered_map<std::string, Signal> signals;
            std::unordered_map<std::string, std::size_t> signal_hash;

            std::unordered_map<std::string, std::vector<std::unique_ptr<MenuItem>>> menu_bar;

            static App* instance;
    };
}
