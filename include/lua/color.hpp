
#pragma once

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

namespace chroma {
    struct Color;
}

namespace chroma::lua {

    void register_color_api(lua_State *L) noexcept;

    void push_color(lua_State *L, Color c) noexcept;

}
