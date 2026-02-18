
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class UndoRedoMenuItem : public MenuItem {
      public:
        UndoRedoMenuItem() noexcept = default;
        virtual ~UndoRedoMenuItem() noexcept override = default;

        virtual void menubar() noexcept override final;
        virtual void shortcuts() noexcept override final;
    };
} // namespace chroma
