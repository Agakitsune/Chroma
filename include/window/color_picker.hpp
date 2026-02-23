/**
 * @file color_picker.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "color.hpp"
#include "system/signal.hpp"
#include "window.hpp"

namespace chroma {

    /**
     * @brief Color Picker window
     * 
     */
    class ColorPickerWindow : public Window {
      public:
        ColorPickerWindow() noexcept;
        virtual ~ColorPickerWindow() noexcept = default;

        virtual void ready() noexcept override final;
        virtual void display() noexcept override final;

        Color main_color;
        Color second_color;

      private:
        void _on_main_color_selected(const Color &clr) noexcept;
        void _on_second_color_selected(const Color &clr) noexcept;
    };
} // namespace chroma
