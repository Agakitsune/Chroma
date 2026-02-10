
#include "menu/fileformat.hpp"

#include <cstring>
#include <cstdint>

namespace chroma {

    bool is_image(const std::string &ext) noexcept
    {
        return fetch_format(ext) != FileFormat::Count;
    }

    FileFormat fetch_format(const std::string &ext) noexcept
    {
        uint32_t i = 0;
        for (const char *e : extensions) {
            int res = std::strcmp(ext.c_str(), e);
            if (res == 0) {
                return formats[i];
            }
            i++;
        }
        return FileFormat::Count;
    }

}
