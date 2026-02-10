
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class FlipMenuItem : public MenuItem {
        public:
            FlipMenuItem() noexcept = default;
            ~FlipMenuItem() noexcept = default;

            virtual void menubar() noexcept override final;
    };
}
