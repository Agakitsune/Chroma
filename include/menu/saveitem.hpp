
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class SaveMenuItem : public MenuItem {
        char name[1024];
        // uint32_t w;
        // uint32_t h;

        public:
            SaveMenuItem() noexcept = default;
            ~SaveMenuItem() noexcept = default;

            virtual void menubar() noexcept override final;
            virtual void display() noexcept override final;
    };
}
