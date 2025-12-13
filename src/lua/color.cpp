
#include "lua/color.hpp"
#include "lua/api.hpp"

#include "color.hpp"

namespace chroma::lua {

    static int color_new(lua_State *L) noexcept {
        Color *c = (Color*)lua_newuserdata(L, sizeof(Color));

        c->r = luaL_checknumber(L, 1);
        c->g = luaL_checknumber(L, 2);
        c->b = luaL_checknumber(L, 3);
        if (lua_type(L, 4) == LUA_TNUMBER) {
            c->a = lua_tonumber(L, 4);
        } else {
            c->a = 1.0f;
        }

        luaL_getmetatable(L, "chroma.Color");
        lua_setmetatable(L, -2);

        return 1;
    }

    static int color_index(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");
        const char *key = luaL_checkstring(L, 2);
    
        if (strcmp(key, "r") == 0) lua_pushnumber(L, c->r);
        else if (strcmp(key, "g") == 0) lua_pushnumber(L, c->g);
        else if (strcmp(key, "b") == 0) lua_pushnumber(L, c->b);
        else if (strcmp(key, "a") == 0) lua_pushnumber(L, c->a);
        else {
            luaL_getmetatable(L, "chroma.Color");
            lua_getfield(L, -1, key);  // methods
        }
    
        return 1;
    }

    static int color_newindex(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");
        const char *key = luaL_checkstring(L, 2);
        float v = luaL_checknumber(L, 3);
    
        if (strcmp(key, "r") == 0) c->r = v;
        else if (strcmp(key, "g") == 0) c->g = v;
        else if (strcmp(key, "b") == 0) c->b = v;
        else if (strcmp(key, "a") == 0) c->a = v;
        else luaL_error(L, "unknown field '%L'", key);
    
        return 0;
    }

    static int color_clone(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");
        Color *n = (Color*)lua_newuserdata(L, sizeof(Color));
        *n = *c;
    
        luaL_getmetatable(L, "chroma.Color");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int color_toRGBA(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");
        lua_pushinteger(L, (int)(c->r * 255));
        lua_pushinteger(L, (int)(c->g * 255));
        lua_pushinteger(L, (int)(c->b * 255));
        lua_pushinteger(L, (int)(c->a * 255));
        return 4;
    }

    static int color_tou32(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");
        uint32_t u = c->to_u32();
        lua_pushinteger(L, u);
        return 1;
    }

    static int color_mul(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");
        float v = luaL_checknumber(L, 2);
    
        Color *r = (Color*)lua_newuserdata(L, sizeof(Color));
        r->r = c->r * v;
        r->g = c->g * v;
        r->b = c->b * v;
        r->a = c->a;
    
        luaL_getmetatable(L, "chroma.Color");
        lua_setmetatable(L, -2);
    
        return 1;
    }

    static int color_tostring(lua_State *L) {
        Color *c = (Color*)luaL_checkudata(L, 1, "chroma.Color");

        char out[1024] = {0};
        sprintf(
            out,
            "Color(r=%.3f, g=%.3f, b=%.3f, a=%.3f)",
            c->r, c->g, c->b, c->a
        );

        lua_pushstring(L, out);
    
        return 1;
    }

    static const luaL_Reg color_methods[] = {
        {"clone", color_clone},
        {"toRGBA", color_toRGBA},
        {"toU32", color_tou32},
        {NULL, NULL}
    };

    static const luaL_Reg color_lib[] = {
        {"new", color_new},
        {NULL, NULL}
    };

    static int luaopen_color(lua_State *L) {
        luaL_newmetatable(L, "chroma.Color");
    
        lua_pushcfunction(L, color_index);
        lua_setfield(L, -2, "__index");
    
        lua_pushcfunction(L, color_newindex);
        lua_setfield(L, -2, "__newindex");
    
        lua_pushcfunction(L, color_mul);
        lua_setfield(L, -2, "__mul");

        lua_pushcfunction(L, color_tostring);
        lua_setfield(L, -2, "__tostring");
    
        luaL_setfuncs(L, color_methods, 0);
        return 1;
    }

    void register_color_api(lua_State *L) noexcept
    {
        luaopen_color(L);
        lua_pop(L, 1); // Pop Chroma metatable

        luaL_newlib(L, color_lib);
        lua_setfield(L, -2, "Color");

    }

    void push_color(lua_State *L, Color c) noexcept
    {
        Color *u = (Color*)lua_newuserdata(L, sizeof(Color));
        *u = c;

        luaL_getmetatable(L, "chroma.Color");
        lua_setmetatable(L, -2);
    }

} // namespace chroma
