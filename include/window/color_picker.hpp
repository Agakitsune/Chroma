
#pragma once

#include "window.hpp"
#include "color.hpp"
#include "system/signal.hpp"

namespace chroma {

    class ColorPickerWindow : public Window {
        public:
            ColorPickerWindow() noexcept;
            virtual ~ColorPickerWindow() noexcept = default; 

            virtual void ready() noexcept override final;
            virtual void display() noexcept override final;
        
            Color main_color;
            Color second_color;

            Signal<Color> color_picked;
        
        private:
            void _on_main_color_selected(const Color &clr);
    };
}
