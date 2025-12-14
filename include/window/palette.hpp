
#pragma once

#include "window.hpp"

#include "SDL3/SDL.h"

#include "color.hpp"

#include <string>
#include <vector>

namespace chroma {

    class PaletteWindow : public Window {
        std::vector<Color> palette;
        uint32_t selected = 0;

        public:
            PaletteWindow() noexcept;
            virtual ~PaletteWindow() noexcept = default; 

            virtual void display() noexcept override final;

            void add_color(const Color &color) noexcept;
    };

}
