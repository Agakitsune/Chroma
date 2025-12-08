
#pragma once

#include "window.hpp"

#include "color.hpp"

namespace chroma {

    class ColorPickerWindow : public Window {
        public:
            ColorPickerWindow();

            bool render();
        
            Color main_color;
            Color second_color;
    };
}
