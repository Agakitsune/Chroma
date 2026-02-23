/**
 * @file app.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "SDL3/SDL.h"
// #include "SDL3/SDL_gpu.h"
#include "color.hpp"

#include "window/window.hpp"

#include "system/signal.hpp"

#include "menu/menuitem.hpp"

#include <memory>
#include <unordered_map>

namespace chroma {

    /**
     * @brief The core of the application
     * 
     */
    class App {
      public:
        App() noexcept = default;
        ~App() noexcept;

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        App(App &&) = delete;
        App &operator=(App &&) = delete;

        int init() noexcept;
        int run() noexcept;

        /**
         * @brief Add a Signal
         * 
         * @ref Signal
         * 
         * Add a Signal to the application that can be used by Windows to communicate with others.
         * 
         * The template parameters define the type of the Signal arguments, THIS IS IMPORTANT, those parameters need to match through call of `connect_signal` and `emit_signal`
         * 
         * ```
         * struct A {
         *      void _on_test_color(Color c);
         *      void _on_invalid_test_color(const Color &c);
         * };
         * 
         * A instance;
         * 
         * add_signal<Color>("test_color"); // Add a Signal with type Color
         * 
         * connect_signal("test_color", &instance, &A::_on_test_color); // Color type matches, will connect
         * connect_signal("test_color", &instance, &A::_on_invalid_test_color); // Color type mismatch, won't connect
         * 
         * emit_signal<Color>("test_color", BLACK); // Force down type, will emit
         * emit_signal<const Color &>("test_color", BLACK); // Force down type mismatch, won't emit
         * emit_signal("test_color", BLACK); // Deduce type, may not work
         * ```
         * 
         * @tparam A The parameters of the signal, THIS IS IMPORTANT
         * @param name The name of the signal
         */
        template <typename... A> void add_signal(const std::string &name) {
            std::size_t hash = typeid(void (*)(std::decay_t<A>...)).hash_code();
            signals.insert_or_assign(name, Signal());
            signal_hash.insert_or_assign(name, hash);
        }

        /**
         * @brief Connect a signal
         * 
         * @ref Signal
         * 
         * Connect a member method to a signal, the argument in the function MUST match the type of the Signal at creation, passing down the template arguments won't work here
         * 
         * * ```
         * struct A {
         *      void _on_test_color(Color c);
         *      void _on_invalid_test_color(const Color &c);
         * };
         * 
         * A instance;
         * 
         * add_signal<Color>("test_color"); // Add a Signal with type Color
         * 
         * connect_signal("test_color", &instance, &A::_on_test_color); // Color type matches, will connect
         * connect_signal("test_color", &instance, &A::_on_invalid_test_color); // Color type mismatch, won't connect
         * ```
         * 
         * @tparam O Object type
         * @tparam A The parameters of the method, must match the Signal parameters
         * @param name The signal name
         * @param object The object
         * @param func The member function
         */
        template <typename O, typename... A>
        void connect_signal(const std::string &name, O *object,
                            void (O::*func)(A...)) {
            std::size_t hash = typeid(void (*)(std::decay_t<A>...)).hash_code();
            if (!signals.contains(name)) {
                return;
            }
            if (signal_hash[name] != hash) {
                return;
            }
            signals[name].connect(object, func);
        }

        /**
         * @brief Emit a Signal
         * 
         * @ref Signal
         * 
         * Emit a signal and call every connected method, to ensure no undefined behavior, consider passing down the template arguments
         * 
         * ```
         * emit_signal<Color>("test_color", BLACK); // Force down type, will emit
         * emit_signal("test_color", BLACK); // Deduce type, may not work
         * ```
         * 
         * @tparam A The parameters of the signal
         * @param name The signal name
         * @param args The emitted arguments
         */
        template <typename... A>
        void emit_signal(const std::string &name, A... args) {
            std::size_t hash = typeid(void (*)(std::decay_t<A>...)).hash_code();
            if (!signals.contains(name)) {
                return;
            }
            if (signal_hash[name] != hash) {
                return;
            }
            signals[name].emit(std::forward<A>(args)...);
        }

        /**
         * @brief Add a Menu Item
         * 
         * @ref MenuItem
         * 
         * Add a Menu Item to a Menu in the Menubar
         * 
         * ```
         * add_menu<OpenMenuItem>("File"); // 'File' Menu has now 'Open' Item
         * add_menu<FlipMenuItem>("Edit"); // 'Edit' Menu has now 'Flip' Item
         * ```
         * 
         * @tparam I The type of the Menu, must inherit @ref MenuItem
         * @param menu The name of the Menu
         * @return I* An instance of the MenuItem
         */
        template <typename I> I *add_menu(const std::string &menu) {
            // if (!menu_bar.contains(menu)) {
            //     menu_bar[menu]menu,
            //     std::vector<std::unique_ptr<MenuItem>>());
            // }
            menu_bar[menu].push_back(std::make_unique<I>());
            return (I *)menu_bar[menu].back().get();
        }

        /**
         * @brief Add a Separator to a Menu
         * 
         * Add a Separator for clarity in the menus
         * 
         * ```
         * add_menu<SaveMenuItem>("File");
         * add_separator("File")
         * add_menu<OpenMenuItem>("File");
         * 
         * // Menu will look like this
         * //
         * // Save
         * // ------
         * // Open
         * //
         * ```
         * 
         * @param menu The menu name
         */
        void separator(const std::string &menu) {
            // if (!menu_bar.contains(menu)) {
            //     menu_bar.insert(menu,
            //     std::vector<std::unique_ptr<MenuItem>>());
            // }
            menu_bar[menu].emplace_back(nullptr);
        }

        static App *get_instance() noexcept;
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

        SDL_Window *window = nullptr;

        SDL_GPUDevice *device = nullptr;
        SDL_Renderer *renderer = nullptr;
        // SDL_GPUCommandBuffer *cmd_buffer = nullptr;

        // SDL_GPUSampler *sampler = nullptr;

        bool dockspace_initialized = false;
        bool done = false;
        bool idle = false;

        std::vector<std::unique_ptr<Window>> windows;

        std::unordered_map<std::string, Signal> signals;
        std::unordered_map<std::string, std::size_t> signal_hash;

        std::unordered_map<std::string, std::vector<std::unique_ptr<MenuItem>>>
            menu_bar;

        static App *instance;
    };
} // namespace chroma
