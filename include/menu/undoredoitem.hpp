
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class UndoRedoMenuItem : public MenuItem {
        public:
            UndoRedoMenuItem() noexcept = default;
            ~UndoRedoMenuItem() noexcept = default;

            virtual void menubar() noexcept override final;
            virtual void shortcuts() noexcept override final;
    };
}
