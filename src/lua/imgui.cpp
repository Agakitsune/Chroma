
#include "lua/imgui.hpp"
#include "lua/imgui/vec2.hpp"

#include "lua/api.hpp"

namespace chroma::lua {

    static int begin_imgui(lua_State *L) noexcept
    {
        const char *label = luaL_checkstring(L, 1); // window name
        ImGuiWindowFlags flags = 0;
        if (lua_type(L, 2) == LUA_TNUMBER) {
            flags = lua_tonumber(L, 2);
        }

        bool res = ImGui::Begin(label, nullptr, flags);

        lua_pushboolean(L, res);
        return 1;
    }

    static int text_imgui(lua_State *L) noexcept
    {
        const char *text = luaL_checkstring(L, 1);
        ImGui::Text(text);
        return 0;
    }

    static int pushid_imgui(lua_State *L) noexcept
    {
        if (lua_isinteger(L, 1)) {
            int id = luaL_checkinteger(L, 1);
            ImGui::PushID(id);
        } else if (lua_isstring(L, 1)) {
            const char* id_str = luaL_checkstring(L, 1);
            ImGui::PushID(id_str);
        } else {
            return luaL_error(L, "Expected integer or string for ImGui.PushID");
        }
        return 0;
    }

    static int popid_imgui(lua_State *L) noexcept
    {
        ImGui::PopID();
        return 0;
    }

    static int end_imgui(lua_State *L) noexcept
    {
        ImGui::End();
        return 0;
    }

    static const luaL_Reg imgui_functions[] = {
        {"Begin", begin_imgui},
        {"End", end_imgui},

        {"Text", text_imgui},

        {"PushID", pushid_imgui},
        {"PopID", popid_imgui},

        {NULL, NULL}
    };
    
    void register_imgui_api(lua_State *L) noexcept
    {
        lua_getfield(L, -1, "ImGui");
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setfield(L, -2, "ImGui");
            lua_getfield(L, -1, "ImGui");
        }

        register_vec2_api(L);
        lua_setfield(L, -2, "Vec2");

        luaL_setfuncs(L, imgui_functions, 0);

        lua_pop(L, 1); // Pop ImGui table
    }

} // namespace chroma::lua
