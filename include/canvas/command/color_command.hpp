
#pragma once

#include "command.hpp"

#include "color.hpp"

namespace chroma {

    struct Canvas;

    class ColorCommand : public ICommand {
        public:
            Color main;
            Color second;

            ColorCommand() noexcept = default;

            virtual void set_main_color(const Color &color) noexcept override final
            {
                main = color;
            }

            virtual void set_second_color(const Color &color) noexcept override final
            {
                second = color;
            }

            virtual Color get_main_color() noexcept override final
            {
                return main;
            }

            virtual Color get_second_color() noexcept override final
            {
                return second;
            }
    };

}
