
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

    class ExitMenuItem : public MenuItem {
        public:
            ExitMenuItem() noexcept = default;
            ~ExitMenuItem() noexcept = default;

            virtual void menubar() noexcept override final;
            virtual void shortcuts() noexcept override final;            
    };
}
