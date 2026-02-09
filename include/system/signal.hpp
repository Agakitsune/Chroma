
#pragma once

#include <functional>
#include <vector>

#include <iostream>

namespace chroma {

    class Signal {
        std::vector<void(*)()> connected;
        std::vector<void*> objects;

        public:
            Signal() noexcept = default;
            ~Signal() noexcept = default;

            Signal(const Signal &other) noexcept = default;
            Signal(Signal &&other) noexcept = default;

            Signal &operator=(const Signal &other) noexcept = default;
            Signal &operator=(Signal &&other) noexcept = default;

            template<typename O, typename... Args>
            void connect(O *object, void (O::*func)(Args...)) noexcept
            {
                connected.push_back((void(*)())func);
                objects.push_back(object);
            }

            void disconnect(void *object) noexcept
            {
                auto it = objects.begin();
                for (; it != objects.end(); it++) {
                    if (*it == object) {
                        break;
                    }
                }
                
                size_t diff = it - objects.begin();
                objects.erase(it);
                connected.erase(connected.begin() + diff);
            }

            template<typename... Args>
            void emit(Args&&... args) const noexcept
            {
                for (size_t i = 0; i < connected.size(); i++) {
                    void(*fn)(void*, Args...) = (void(*)(void*, Args...))connected[i];
                    fn(objects[i], std::forward<Args>(args)...);
                }
            }
    };
}
