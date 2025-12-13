
#pragma once

#include "lua.hpp"

#include "imgui.h"

namespace chroma::lua {

    void register_imgui_api(lua_State *L) noexcept;

}
