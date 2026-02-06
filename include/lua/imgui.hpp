
#pragma once

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "imgui.h"

namespace chroma::lua {

    void register_imgui_api(lua_State *L) noexcept;

}
