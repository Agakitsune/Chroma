
#pragma once

#include "imgui.h"

#include <cstdint>

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

        constexpr uint32_t to_u32() const noexcept {
            return
                ((uint32_t)(a * 255.0f) << 24) |
                ((uint32_t)(b * 255.0f) << 16) |
                ((uint32_t)(g * 255.0f) << 8)  |
                ((uint32_t)(r * 255.0f) << 0);
        }

        void upload(uint8_t *dst) const noexcept {
            dst[0] = (uint8_t)(r * 255.0f);
            dst[1] = (uint8_t)(g * 255.0f);
            dst[2] = (uint8_t)(b * 255.0f);
            dst[3] = (uint8_t)(a * 255.0f);
        }

        void download(const uint8_t *src) noexcept {
            r = src[0] / 255.0f;
            g = src[1] / 255.0f;
            b = src[2] / 255.0f;
            a = src[3] / 255.0f;
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
