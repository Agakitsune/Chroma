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

/**
 * @brief FileFormat enum
 *
 */
enum FileFormat {
    All,
    BMP, // BMP Format (.bmp)
    JPG, // JPG Format (.jpeg, .jpg)
    PNG, // PNG Format (.png)
    TGA, // TGA Format (.tga)

    Count
};

static const char* extensions[] = { "All formats", ".bmp", ".jpeg",
    ".jpg", ".png", ".tga" };

static FileFormat formats[] = { All, BMP, JPG, JPG, PNG, TGA };

/**
 * @brief Is the current extension supported
 *
 * @param ext The extension
 * @return true The extension is supported
 * @return false Thd extension isn't supported
 */
bool is_image(const std::string& ext) noexcept;

/**
 * @brief Convert the extension to a @ref FileFormat
 *
 * @param ext The extension
 * @return FileFormat The associated @ref FileFormat, @ref FileFormat::Count if
 * not found
 */
FileFormat fetch_format(const std::string& ext) noexcept;

} // namespace chroma
