
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class NewMenuItem : public MenuItem {
        uint32_t w;
        uint32_t h;

        public:
            NewMenuItem() noexcept = default;
            ~NewMenuItem() noexcept = default;

            virtual void menubar() noexcept override final;
            virtual void display() noexcept override final;
    };
}
