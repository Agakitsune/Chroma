
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class FlipMenuItem : public MenuItem {
      public:
        FlipMenuItem() noexcept = default;
        virtual ~FlipMenuItem() noexcept override = default;

        virtual void menubar() noexcept override final;
        virtual void shortcuts() noexcept override final;
    };
} // namespace chroma
