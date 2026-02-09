
#include "lua/api.hpp"
#include "lua/color.hpp"
#include "lua/imgui.hpp"

#include <iostream>

namespace chroma::lua {

    void print_stack(lua_State *s) noexcept {
        int top = lua_gettop(s);
        printf("Stack (%d elements):\n", top);
        for (int i = 1; i <= top; i++) {
            int t = lua_type(s, i);
            switch(t) {
                case LUA_TNUMBER:
                    printf("%d: number = %f\n", i, lua_tonumber(s, i));
                    break;
                case LUA_TSTRING:
                    printf("%d: string = '%s'\n", i, lua_tostring(s, i));
                    break;
                case LUA_TUSERDATA:
                    printf("%d: userdata = %p\n", i, lua_touserdata(s, i));
                    break;
                case LUA_TBOOLEAN:
                    printf("%d: boolean = %s\n", i, lua_toboolean(s,i) ? "true":"false");
                    break;
                case LUA_TTABLE:
                    printf("%d: table\n", i);
                    break;
                default:
                    printf("%d: %s\n", i, lua_typename(s, t));
                    break;
            }
        }
    }

    // static int __new_vec2(lua_State *s) noexcept {
    //     double x = luaL_checknumber(s, 1);
    //     double y = luaL_checknumber(s, 2);

    //     push_vec2(s, ImVec2(x, y));

    //     return 1;
    // }

    // static int __new_vec4(lua_State *s) noexcept {
    //     double x = luaL_checknumber(s, 1);
    //     double y = luaL_checknumber(s, 2);
    //     double z = luaL_checknumber(s, 3);
    //     double w = luaL_checknumber(s, 4);

    //     push_vec4(s, ImVec4(x, y, z, w));

    //     return 1;
    // }

    // static int __new_color(lua_State *s) noexcept {
    //     double x = luaL_checknumber(s, 1);
    //     double y = luaL_checknumber(s, 2);
    //     double z = luaL_checknumber(s, 3);
    //     double w = luaL_checknumber(s, 4);

    //     push_color(s, Color(x, y, z, w));

    //     return 1;
    // }

    void register_chroma_api(lua_State *L) noexcept
    {
        lua_getglobal(L, "Chroma");
        if (lua_isnil(L, -1)) {
            lua_newtable(L);
            lua_setglobal(L, "Chroma");
            lua_getglobal(L, "Chroma");
        }

        register_color_api(L);
        register_imgui_api(L);
        
        lua_pop(L, 1); // Pop Chroma table
    }

    // static int lua_panic(lua_State *s) {
    //     const char *msg = lua_tostring(s, -1);

    //     fprintf(stderr, "\n=== LUA PANIC ===\n");
    //     if (msg)
    //         fprintf(stderr, "Error: %s\n", msg);

    //     luaL_traceback(s, s, NULL, 1);
    //     fprintf(stderr, "%s\n", lua_tostring(s, -1));

    //     abort();
    // }

    // LuaAPI::LuaAPI() noexcept
    // {
    //     state = luaL_newstate();

    //     lua_atpanic(state, lua_panic);

    //     luaL_openlibs(state);
    // }

    // LuaAPI::~LuaAPI() noexcept
    // {
    //     lua_close(state);
    // }

    // std::unique_ptr<LuaAPI> LuaAPI::instance;

    // LuaAPI* LuaAPI::get_instance() noexcept
    // {
    //     if (!instance) {
    //         instance = std::unique_ptr<LuaAPI>(new LuaAPI());
    //     }
    //     return instance.get();
    // }

    // void LuaAPI::register_chroma_api() noexcept
    // {
    //     register_color_api();
    // }

    // void register_lua(lua_State *s) noexcept {
    //     lua_atpanic(s, lua_panic);

    //     luaL_openlibs(s);

    //     luaL_requiref(s, "Color", luaopen_color, 1);
    //     lua_pop(s, 1);
    //     // luaopen_color(s);
    //     // lua_setglobal(s, "Color");
    //     // lua_pushcfunction(s, __new_vec2);
    //     // lua_setglobal(s, "vec2");
        
    //     // lua_pushcfunction(s, __new_vec4);
    //     // lua_setglobal(s, "vec4");
        
    //     // lua_pushcfunction(s, __new_color);
    //     // lua_setglobal(s, "color");
    // }

    // static void set_table_field_number(lua_State *s, const char *name, lua_Number n) noexcept {
    //     lua_pushstring(s, name);
    //     lua_pushnumber(s, n);
    //     lua_settable(s, -3);
    // }

    // void push_vec2(lua_State *s, ImVec2 v) noexcept {
    //     lua_newtable(s);
    //     set_table_field_number(s, "x", v.x);
    //     set_table_field_number(s, "y", v.y);
    // }

    // void push_vec4(lua_State *s, ImVec4 v) noexcept {
    //     lua_newtable(s);
    //     set_table_field_number(s, "x", v.x);
    //     set_table_field_number(s, "y", v.y);
    //     set_table_field_number(s, "z", v.z);
    //     set_table_field_number(s, "w", v.w);
    // }

    // void push_color(lua_State *s, Color c) noexcept {
    //     lua_newtable(s);
    //     set_table_field_number(s, "r", c.r);
    //     set_table_field_number(s, "g", c.g);
    //     set_table_field_number(s, "b", c.b);
    //     set_table_field_number(s, "a", c.a);
    // }
    
}
