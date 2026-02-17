/**
 * @file fileformat.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief 
 * @version 0.1
 * @date 2026-02-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include <string>

namespace chroma {

    enum FileFormat {
        All,
        BMP,
        JPG,
        PNG,
        TGA,

        Count
    };

    static const char* extensions[] = {
        "All formats",
        ".bmp",
        ".jpeg",
        ".jpg",
        ".png",
        ".tga"
    };

    static FileFormat formats[] = {
        All,
        BMP,
        JPG,
        JPG,
        PNG,
        TGA
    };

    bool is_image(const std::string &ext) noexcept;
    FileFormat fetch_format(const std::string &ext) noexcept;

}
