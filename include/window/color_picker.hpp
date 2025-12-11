
#pragma once

#include "window.hpp"

#include "color.hpp"

namespace chroma {

    class ColorPickerWindow : public Window {
        public:
            ColorPickerWindow() noexcept;

            virtual void display() noexcept override final;
        
            Color main_color;
            Color second_color;
    };
}
