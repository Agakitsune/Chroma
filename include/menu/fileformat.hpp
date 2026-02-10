
#pragma once

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

}
