/**
 * @file exititem.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
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
} // namespace chroma
