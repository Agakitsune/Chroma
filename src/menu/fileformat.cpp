/**
 * @file fileformat.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Fetching file format
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "menu/fileformat.hpp"

#include <cstdint>
#include <cstring>

namespace chroma {

bool is_image(const std::string& ext) noexcept
{
    return fetch_format(ext) != FileFormat::Count;
}

FileFormat fetch_format(const std::string& ext) noexcept
{
    uint32_t i = 0;
    for (const char* e : extensions) {
        int res = std::strcmp(ext.c_str(), e);
        if (res == 0) {
            return formats[i];
        }
        i++;
    }
    return FileFormat::Count;
}

} // namespace chroma
