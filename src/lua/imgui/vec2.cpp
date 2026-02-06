
#include "lua/imgui/vec2.hpp"
#include "lua/api.hpp"

namespace chroma::lua {
    
    int vec2_new(lua_State *s) noexcept {
        ImVec2 *c = (ImVec2*)lua_newuserdata(s, sizeof(ImVec2));

        c->x = luaL_checknumber(s, 1);
        c->y = luaL_checknumber(s, 2);

        luaL_getmetatable(s, "chroma.imgui.ImVec2");
        lua_setmetatable(s, -2);

        return 1;
    }

    static int vec2_index(lua_State *s) noexcept {
        ImVec2 *c = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");
        const char *key = luaL_checkstring(s, 2);
    
        if (strcmp(key, "x") == 0) lua_pushnumber(s, c->x);
        else if (strcmp(key, "y") == 0) lua_pushnumber(s, c->y);
        else {
            luaL_getmetatable(s, "chroma.imgui.ImVec2");
            lua_getfield(s, -1, key);  // methods
        }
    
        return 1;
    }

    static int vec2_newindex(lua_State *s) noexcept {
        ImVec2 *c = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");
        const char *key = luaL_checkstring(s, 2);
        float v = luaL_checknumber(s, 3);
    
        if (strcmp(key, "x") == 0) c->x = v;
        else if (strcmp(key, "y") == 0) c->y = v;
        else luaL_error(s, "unknown field '%s'", key);
    
        return 0;
    }

    static int vec2_clone(lua_State *s) noexcept {
        ImVec2 *c = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");
        ImVec2 *n = (ImVec2*)lua_newuserdata(s, sizeof(ImVec2));
        *n = *c;
    
        luaL_getmetatable(s, "chroma.imgui.ImVec2");
        lua_setmetatable(s, -2);
        return 1;
    }

    static int vec2_add(lua_State *s) noexcept {
        ImVec2 *a = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");
        ImVec2 *b = (ImVec2*)luaL_checkudata(s, 2, "chroma.imgui.ImVec2");
    
        ImVec2 *r = (ImVec2*)lua_newuserdata(s, sizeof(ImVec2));
        r->x = a->x + b->x;
        r->y = a->y + b->y;
    
        luaL_getmetatable(s, "chroma.imgui.ImVec2");
        lua_setmetatable(s, -2);
    
        return 1;
    }

    static int vec2_sub(lua_State *s) noexcept {
        ImVec2 *a = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");
        ImVec2 *b = (ImVec2*)luaL_checkudata(s, 2, "chroma.imgui.ImVec2");
    
        ImVec2 *r = (ImVec2*)lua_newuserdata(s, sizeof(ImVec2));
        r->x = a->x - b->x;
        r->y = a->y - b->y;
    
        luaL_getmetatable(s, "chroma.imgui.ImVec2");
        lua_setmetatable(s, -2);
    
        return 1;
    }

    static int vec2_mul(lua_State *s) noexcept {
        ImVec2 *c = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");
        float v = luaL_checknumber(s, 2);
    
        ImVec2 *r = (ImVec2*)lua_newuserdata(s, sizeof(ImVec2));
        r->x = c->x * v;
        r->y = c->y * v;
    
        luaL_getmetatable(s, "chroma.imgui.ImVec2");
        lua_setmetatable(s, -2);
    
        return 1;
    }

    static int vec2_tostring(lua_State *s) noexcept {
        ImVec2 *c = (ImVec2*)luaL_checkudata(s, 1, "chroma.imgui.ImVec2");

        char out[1024] = {0};
        sprintf(
            out,
            "ImVec2(x=%.3f, y=%.3f)",
            c->x, c->y
        );

        lua_pushstring(s, out);
    
        return 1;
    }

    static const luaL_Reg vec2_methods[] = {
        {"clone", vec2_clone},
        {NULL, NULL}
    };

    static const luaL_Reg vec2_functions[] = {
        {"new", vec2_new},
        {NULL, NULL}
    };

    static int luaopen_vec2(lua_State *s) noexcept {
        luaL_newmetatable(s, "chroma.imgui.ImVec2");
    
        lua_pushcfunction(s, vec2_index);
        lua_setfield(s, -2, "__index");
    
        lua_pushcfunction(s, vec2_newindex);
        lua_setfield(s, -2, "__newindex");

        lua_pushcfunction(s, vec2_add);
        lua_setfield(s, -2, "__add");

        lua_pushcfunction(s, vec2_sub);
        lua_setfield(s, -2, "__sub");
    
        lua_pushcfunction(s, vec2_mul);
        lua_setfield(s, -2, "__mul");

        lua_pushcfunction(s, vec2_tostring);
        lua_setfield(s, -2, "__tostring");
    
        luaL_setfuncs(s, vec2_methods, 0);
        return 1;
    }

    void register_vec2_api(lua_State *L) noexcept
    {
        luaopen_vec2(L);
        lua_pop(L, 1);; // Pop metatable
        luaL_newlib(L, vec2_functions);
    }

    void push_vec2(lua_State *L, ImVec2 v) noexcept
    {

    }

} // namespace chroma::lua
