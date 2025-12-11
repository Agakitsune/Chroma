#include "lua/api.hpp"

#include <iostream>

namespace chroma {

    static int print_stack(lua_State *s) {
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
        return 0;
    }

    static int color_new(lua_State *s) noexcept {
        Color *c = (Color*)lua_newuserdata(s, sizeof(Color));

        c->r = luaL_checknumber(s, 1);
        c->g = luaL_checknumber(s, 2);
        c->b = luaL_checknumber(s, 3);
        if (lua_type(s, 4) == LUA_TNUMBER) {
            c->a = lua_tonumber(s, 4);
        } else {
            c->a = 1.0f;
        }

        luaL_getmetatable(s, "chroma.Color");
        lua_setmetatable(s, -2);

        return 1;
    }

    static int color_index(lua_State *s) {
        Color *c = (Color*)luaL_checkudata(s, 1, "chroma.Color");
        const char *key = luaL_checkstring(s, 2);
    
        if (strcmp(key, "r") == 0) lua_pushnumber(s, c->r);
        else if (strcmp(key, "g") == 0) lua_pushnumber(s, c->g);
        else if (strcmp(key, "b") == 0) lua_pushnumber(s, c->b);
        else if (strcmp(key, "a") == 0) lua_pushnumber(s, c->a);
        else {
            luaL_getmetatable(s, "chroma.Color");
            lua_getfield(s, -1, key);  // methods
        }
    
        return 1;
    }

    static int color_newindex(lua_State *s) {
        Color *c = (Color*)luaL_checkudata(s, 1, "chroma.Color");
        const char *key = luaL_checkstring(s, 2);
        float v = luaL_checknumber(s, 3);
    
        if (strcmp(key, "r") == 0) c->r = v;
        else if (strcmp(key, "g") == 0) c->g = v;
        else if (strcmp(key, "b") == 0) c->b = v;
        else if (strcmp(key, "a") == 0) c->a = v;
        else luaL_error(s, "unknown field '%s'", key);
    
        return 0;
    }

    static int color_clone(lua_State *s) {
        Color *c = (Color*)luaL_checkudata(s, 1, "chroma.Color");
        Color *n = (Color*)lua_newuserdata(s, sizeof(Color));
        *n = *c;
    
        luaL_getmetatable(s, "chroma.Color");
        lua_setmetatable(s, -2);
        return 1;
    }

    static int color_toRGBA(lua_State *s) {
        Color *c = (Color*)luaL_checkudata(s, 1, "chroma.Color");
        lua_pushinteger(s, (int)(c->r * 255));
        lua_pushinteger(s, (int)(c->g * 255));
        lua_pushinteger(s, (int)(c->b * 255));
        lua_pushinteger(s, (int)(c->a * 255));
        return 4;
    }

    static int color_mul(lua_State *s) {
        Color *c = (Color*)luaL_checkudata(s, 1, "chroma.Color");
        float v = luaL_checknumber(s, 2);
    
        Color *r = (Color*)lua_newuserdata(s, sizeof(Color));
        r->r = c->r * v;
        r->g = c->g * v;
        r->b = c->b * v;
        r->a = c->a;
    
        luaL_getmetatable(s, "chroma.Color");
        lua_setmetatable(s, -2);
    
        return 1;
    }

    static int color_tostring(lua_State *s) {
        Color *c = (Color*)luaL_checkudata(s, 1, "chroma.Color");

        char out[1024] = {0};
        sprintf(
            out,
            "Color(r=%.3f, g=%.3f, b=%.3f, a=%.3f)",
            c->r, c->g, c->b, c->a
        );

        lua_pushstring(s, out);
    
        return 1;
    }

    static const luaL_Reg color_methods[] = {
        {"clone", color_clone},
        {"toRGBA", color_toRGBA},
        {NULL, NULL}
    };

    static int luaopen_color(lua_State *s) {
        luaL_newmetatable(s, "chroma.Color");
    
        lua_pushcfunction(s, color_index);
        lua_setfield(s, -2, "__index");
    
        lua_pushcfunction(s, color_newindex);
        lua_setfield(s, -2, "__newindex");
    
        lua_pushcfunction(s, color_mul);
        lua_setfield(s, -2, "__mul");

        lua_pushcfunction(s, color_tostring);
        lua_setfield(s, -2, "__tostring");
    
        luaL_setfuncs(s, color_methods, 0);
    
        lua_pushcfunction(s, color_new);
        return 1;
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

    static int lua_panic(lua_State *s) {
        const char *msg = lua_tostring(s, -1);

        fprintf(stderr, "\n=== LUA PANIC ===\n");
        if (msg)
            fprintf(stderr, "Error: %s\n", msg);

        luaL_traceback(s, s, NULL, 1);
        fprintf(stderr, "%s\n", lua_tostring(s, -1));

        abort();
    }

    void register_lua(lua_State *s) noexcept {
        lua_atpanic(s, lua_panic);

        luaL_openlibs(s);

        luaL_requiref(s, "Color", luaopen_color, 1);
        lua_pop(s, 1);
        // luaopen_color(s);
        // lua_setglobal(s, "Color");
        // lua_pushcfunction(s, __new_vec2);
        // lua_setglobal(s, "vec2");
        
        // lua_pushcfunction(s, __new_vec4);
        // lua_setglobal(s, "vec4");
        
        // lua_pushcfunction(s, __new_color);
        // lua_setglobal(s, "color");
    }

    static void set_table_field_number(lua_State *s, const char *name, lua_Number n) noexcept {
        lua_pushstring(s, name);
        lua_pushnumber(s, n);
        lua_settable(s, -3);
    }

    void push_vec2(lua_State *s, ImVec2 v) noexcept {
        lua_newtable(s);
        set_table_field_number(s, "x", v.x);
        set_table_field_number(s, "y", v.y);
    }

    void push_vec4(lua_State *s, ImVec4 v) noexcept {
        lua_newtable(s);
        set_table_field_number(s, "x", v.x);
        set_table_field_number(s, "y", v.y);
        set_table_field_number(s, "z", v.z);
        set_table_field_number(s, "w", v.w);
    }

    void push_color(lua_State *s, Color c) noexcept {
        lua_newtable(s);
        set_table_field_number(s, "r", c.r);
        set_table_field_number(s, "g", c.g);
        set_table_field_number(s, "b", c.b);
        set_table_field_number(s, "a", c.a);
    }
    
}
