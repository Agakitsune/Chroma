
#pragma once

#include "imgui.h"

namespace chroma {

    struct Color {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 0.0f;

        constexpr Color() noexcept = default;
        constexpr Color(float r, float g, float b, float a = 1.0f) noexcept : r(r), g(g), b(b), a(a) {}

        constexpr operator ImVec4() const noexcept {
            return ImVec4(r, g, b, a);
        }

        constexpr float &operator[](size_t i) noexcept {
            return (&r)[i];
        }

        constexpr float operator[](size_t i) const noexcept {
            return (&r)[i];
        }
    };

    constexpr Color WHITE(1.0f, 1.0f, 1.0f);
    constexpr Color BLACK(0.0f, 0.0f, 0.0f);
    
    constexpr Color RED(1.0f, 0.0f, 0.0f);
    constexpr Color GREEN(0.0f, 1.0f, 0.0f);
    constexpr Color BLUE(0.0f, 0.0f, 1.0f);

    constexpr Color YELLOW(1.0f, 1.0f, 0.0f);
    constexpr Color CYAN(0.0f, 1.0f, 1.0f);
    constexpr Color MAGENTA(1.0f, 0.0f, 1.0f);

}
