
#pragma once

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "imgui.h"

namespace chroma::lua {

    void register_vec2_api(lua_State *L) noexcept;

    void push_vec2(lua_State *L, ImVec2 v) noexcept;

}
