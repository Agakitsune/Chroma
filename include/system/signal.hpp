
#pragma once

#include <functional>
#include <vector>

#include <iostream>

namespace chroma {

    template<typename... Args>
    class Signal {
        std::vector<std::function<void(std::decay_t<Args>...)>> connected;
        // std::vector<void(*)()> connected;

        public:
            Signal() noexcept = default;
            ~Signal() noexcept = default;

            Signal(const Signal &other) noexcept = default;
            Signal(Signal &&other) noexcept = default;

            Signal &operator=(const Signal &other) noexcept = default;
            Signal &operator=(Signal &&other) noexcept = default;

            void connect(const std::function<void(std::decay_t<Args>...)> &func) noexcept {
                connected.emplace_back(std::move(func));
            }

            template<typename... A>
            void emit(A&&... args) const noexcept {
                for (const auto &con : connected) {
                    con(std::forward<A>(args)...);
                }
            }

            // void connect(void (*func)()) noexcept {
            //     connected.emplace_back(std::move(func));
            // }

            // void disconnect(void (*func)()) noexcept {
            //     auto it = connected.begin();
            //     for (; it != connected.end(); it++) {
            //         if (*it == func) {
            //             break;
            //         }
            //     }
                
            //     connected.erase(it);
            // }

            // template <typename... A>
            // void emit(A&&... args) const noexcept {
            //     std::cout << "emit" << std::endl;
            //     for (const auto &con : connected) {
            //         void(*ptr)(A...) = (void(*)(A...))con;
            //         std::cout << ptr << std::endl;
            //         ptr(std::forward<A>(args)...);
            //     }
            // }
    };
}
