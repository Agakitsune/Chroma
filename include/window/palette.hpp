/**
 * @file palette.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief 
 * @version 0.1
 * @date 2026-02-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include "window.hpp"

#include "SDL3/SDL.h"

#include "color.hpp"

#include <string>
#include <vector>

#include "system/signal.hpp"

namespace chroma {

    class PaletteWindow : public Window {
        std::vector<Color> palette;
        uint32_t selected = 0;

        public:
            PaletteWindow() noexcept;
            virtual ~PaletteWindow() noexcept = default; 

            virtual void ready() noexcept override final;
            virtual void display() noexcept override final;

            void add_color(const Color &color) noexcept;
    };

}
