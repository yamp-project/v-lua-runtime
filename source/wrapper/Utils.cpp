#include "Utils.h"

#include <lua.h>
#include <lualib.h>

#include <print>
#include <string>

void lua::Utils::lua_dumptable(lua_State* L, int idx, int level)
{
    std::string levelTab;
    levelTab.insert(0, level, '\t');

    lua_pushnil(L);  /* first key */
    while (lua_next(L, idx) != 0)
    {
        //we need to copy the key and value so lua_tostring will not convert it to other type
        lua_pushvalue(L, -2);

        //check if value is table
        if (lua_istable(L, -2))
        {
            std::string tableName = luaL_tolstring(L, -1, NULL);
            bool isRepetEqual = lua_equal(L, -1, -2);
            if (isRepetEqual)
            {
                std::print("{}{} => RepeatTable\n", levelTab, tableName);
                lua_pop(L, 1);
            }
            else {

                //make it beautifully aligned
                std::print("{}{} => Table\n", levelTab, tableName);
                std::print("{}{{\n", levelTab);



                lua_pop(L, 1);

                //for some reason we need to pass the original table and not the referenced one (recursive call)
                if (!isRepetEqual)
                    lua_dumptable(L, -3, level + 1);

                //make it beautifuly aligned
                std::print("{}}}\n", levelTab);
            }
        }
        else
        {
            //note: lua_tostring will crash if userdata doesn't have any __tostring metaevent implemented
            std::string stackKey(lua_isstring(L, -1) ? lua_tostring(L, -1) : lua_typename(L, lua_type(L, -1)));
            std::string stackValue(lua_isstring(L, -2) ? lua_tostring(L, -2) : lua_typename(L, lua_type(L, -2)));
            std::print("{}{} => {},\n", levelTab, stackKey, stackValue);
        }

        lua_pop(L, 2);
    }
}

void lua::Utils::lua_stacktrace(lua_State* L, const char* stackName)
{
    int stackTop = lua_gettop(L);

    std::print(" --------- Stack Begins: {}---------\n", std::string(stackName));
    for (int i = stackTop; i >= 1; i--)
    {
        int valueType = lua_type(L, i);
        int relativePosition = ((i - stackTop) - 1);

        switch (valueType)
        {
        case LUA_TFUNCTION:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: 'function'\n", std::to_string(i), std::to_string(relativePosition));
            break;
        case LUA_TSTRING:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'\n", std::to_string(i), std::to_string(relativePosition), luaL_checkstring(L, i));
            break;
        case LUA_TBOOLEAN:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'\n", std::to_string(i), std::to_string(relativePosition), (lua_toboolean(L, i) ? ("true") : ("false")));
            break;
        case LUA_TNUMBER:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'\n", std::to_string(i), std::to_string(relativePosition), std::to_string(luaL_checknumber(L, i)));
            break;
        case LUA_TTABLE:
        {
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value '{}'\n", std::to_string(i), std::to_string(relativePosition), lua_typename(L, valueType));
            std::print("\t{{\n");

            //check if the table is the same as the global
            lua_getglobal(L, "_G");
            bool isGlobalTable = lua_equal(L, i, -1);
            lua_pop(L, 1);

            if (isGlobalTable)
                std::print("\t\t_G => GlobalTable\n");
            else
                lua_dumptable(L, i, 2);

            std::print("\t}}\n");
            /*lua_pushnil(L);
            while (lua_next(L, i) != 0)
            {
            Core->LogInfo("%s => %s", lua_tostring(L, -2), lua_tostring(L, -1));
            lua_pop(L, 1);
            }*/

            break;
        }
        default:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value type: {}\n", i, relativePosition, lua_typename(L, valueType));
            break;
        }
    }
    std::print(" --------- Stack Ends: {} ---------\n", stackName);
}

#include <assert.h>

int lua::Utils::traceback(lua_State* L) {
    assert(lua_checkstack(L, 3));
    int top = lua_gettop(L);
    int bottom = 1;
    lua_getglobal(L, "tostring");
    for (int i = top; i >= bottom; i--)
    {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_pcall(L, 1, 1, 0);
        const char* str = lua_tostring(L, -1);
        if (str) {
            printf("%s\n", str);
        }
        else {
            printf("%s\n", luaL_typename(L, i));
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return 1;
}