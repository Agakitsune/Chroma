/**
 * @file color.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "imgui.h"

#include <cstdint>
#include <iostream>

namespace chroma {

    /**
     * @brief 4 Float Color
     * 
     */
    struct Color {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 0.0f;

        constexpr Color() noexcept = default;
        constexpr Color(float r, float g, float b, float a = 1.0f) noexcept
            : r(r), g(g), b(b), a(a) {}

        constexpr operator ImVec4() const noexcept {
            return ImVec4(r, g, b, a);
        }

        constexpr float &operator[](size_t i) noexcept { return (&r)[i]; }

        constexpr float operator[](size_t i) const noexcept { return (&r)[i]; }

        constexpr uint32_t to_u32() const noexcept {
            return ((uint32_t)(a * 255.0f) << 24) |
                   ((uint32_t)(b * 255.0f) << 16) |
                   ((uint32_t)(g * 255.0f) << 8) |
                   ((uint32_t)(r * 255.0f) << 0);
        }

        void upload(float *dst) const noexcept {
            dst[0] = r;
            dst[1] = g;
            dst[2] = b;
            dst[3] = a;
        }

        void download(const float *src) noexcept {
            r = src[0];
            g = src[1];
            b = src[2];
            a = src[3];
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

    inline std::ostream &operator<<(std::ostream &s, const Color &c) noexcept {
        s << "r: " << c.r;
        s << "g: " << c.g;
        s << "b: " << c.b;
        s << "a: " << c.a;
        return s;
    }

    // Transparent color
    constexpr Color MASK(0.0f, 0.0f, 0.0f, 0.0f);

    constexpr Color WHITE(1.0f, 1.0f, 1.0f); // White color
    constexpr Color BLACK(0.0f, 0.0f, 0.0f); // Black Color

    constexpr Color RED(1.0f, 0.0f, 0.0f); // Red Color
    constexpr Color GREEN(0.0f, 1.0f, 0.0f); // Green Color
    constexpr Color BLUE(0.0f, 0.0f, 1.0f); // Blue Color

    constexpr Color YELLOW(1.0f, 1.0f, 0.0f); // Yellow Color
    constexpr Color CYAN(0.0f, 1.0f, 1.0f); // Cyan Color
    constexpr Color MAGENTA(1.0f, 0.0f, 1.0f); // Magenta Color

} // namespace chroma
