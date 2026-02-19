
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class NewLayerMenuItem : public MenuItem {
      public:
        NewLayerMenuItem() noexcept = default;
        virtual ~NewLayerMenuItem() noexcept override = default;

        virtual void menubar() noexcept override final;
        virtual void action() noexcept override final;
        virtual void shortcuts() noexcept override final;
    };
} // namespace chroma
