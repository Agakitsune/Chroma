
#pragma once

#include "color.hpp"

#include "imgui.h"

#include "lua.hpp"

namespace chroma {

    void register_lua(lua_State *s) noexcept;

    void push_vec2(lua_State *s, ImVec2 v) noexcept;
    void push_vec4(lua_State *s, ImVec4 v) noexcept;
    void push_color(lua_State *s, Color c) noexcept;

}
