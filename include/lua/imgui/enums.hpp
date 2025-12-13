
#pragma once

#include "lua.hpp"

// #include "imgui.h"

namespace chroma::lua {

    void register_enum_api(lua_State *L) noexcept;

    // void push_vec2(lua_State *L, ImVec2 v) noexcept;

}
