#include "utils.h"

#include <yamp-sdk/sdk.h>
#include <yamp-sdk/cpp/any_value.h>

#include <lua.h>
#include <lualib.h>
#include <assert.h>
#include <string>
#include <format>

int lua::utils::luaL_ref(lua_State* L, int t)
{
    assert(t == LUA_REGISTRYINDEX);
    int r = lua_ref(L, -1);
    lua_pop(L, 1);
    return r;
}

void lua::utils::lua_dumptable(lua_State* L, int idx, int level)
{
    std::string levelTab;
    levelTab.insert(0, level, '\t');

    lua_pushnil(L);  /* first key */
    while (lua_next(L, idx) != 0)
    {
        // we need to copy the key and value so lua_tostring will not convert it to other type
        lua_pushvalue(L, -2);

        // check if value is table
        if (lua_istable(L, -2))
        {
            std::string tableName = luaL_tolstring(L, -1, NULL);

            bool isRepetEqual = lua_equal(L, -1, -2);
            if (isRepetEqual)
            {
                printf("%s\n", std::format("{}{} => RepeatTable", levelTab, tableName).c_str());
                lua_pop(L, 1);
            }
            else
            {
                // make it beautifully aligned
                printf("%s\n", std::format("{}{} => Table", levelTab, tableName).c_str());
                printf("%s\n", std::format("{}{{", levelTab).c_str());

                lua_pop(L, 1);

                // for some reason we need to pass the original table and not the referenced one (recursive call)
                if (!isRepetEqual)
                {
                    lua_dumptable(L, -3, level + 1);
                }

                // make it beautifuly aligned
                printf("%s\n", std::format("{}}}", levelTab).c_str());
            }
        }
        else
        {
            // note: lua_tostring will crash if userdata doesn't have any __tostring metaevent implemented
            std::string stackKey(lua_isstring(L, -1) ? lua_tostring(L, -1) : lua_typename(L, lua_type(L, -1)));
            std::string stackValue(lua_isstring(L, -2) ? lua_tostring(L, -2) : lua_typename(L, lua_type(L, -2)));
            printf("%s\n", std::format("{}{} => {},", levelTab, stackKey, stackValue).c_str());
        }

        lua_pop(L, 2);
    }
}

void lua::utils::lua_stacktrace(lua_State* L, const char* stackName)
{
    int stackTop = lua_gettop(L);
    printf("%s\n", std::format(" --------- Stack Begins: {}---------\n", std::string(stackName)).c_str());

    for (int i = stackTop; i >= 1; i--)
    {
        int valueType = lua_type(L, i);
        int relativePosition = ((i - stackTop) - 1);

        switch (valueType)
        {
        case LUA_TFUNCTION:
            printf("%s\n", std::format("\tAbsolute: {} ~~ Relative: {} ~~\t Value: 'function'", std::to_string(i), std::to_string(relativePosition)).c_str());
            break;
        case LUA_TSTRING:
            printf("%s\n", std::format("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'", std::to_string(i), std::to_string(relativePosition), luaL_checkstring(L, i)).c_str());
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", std::format("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'", std::to_string(i), std::to_string(relativePosition), (lua_toboolean(L, i) ? ("true") : ("false"))).c_str());
            break;
        case LUA_TNUMBER:
            printf("%s\n", std::format("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'", std::to_string(i), std::to_string(relativePosition), std::to_string(luaL_checknumber(L, i))).c_str());
            break;
        case LUA_TTABLE:
        {
            printf("%s\n", std::format("\tAbsolute: {} ~~ Relative: {} ~~\t Value '{}'", std::to_string(i), std::to_string(relativePosition), lua_typename(L, valueType)).c_str());
            printf("\t{{\n");

            // check if the table is the same as the global
            lua_getglobal(L, "_G");
            bool isGlobalTable = lua_equal(L, i, -1);
            lua_pop(L, 1);

            if (isGlobalTable)
            {
                printf("\t\t_G => GlobalTable\n");
            }
            else
            {
                lua_dumptable(L, i, 2);
            }

            printf("\t}}\n");
            /*lua_pushnil(L);
            while (lua_next(L, i) != 0)
            {
            Core->LogInfo("%s => %s", lua_tostring(L, -2), lua_tostring(L, -1));
            lua_pop(L, 1);
            }*/

            break;
        }
        default:
            printf("%s\n", std::format("\tAbsolute: {} ~~ Relative: {} ~~\t Value type: {}", i, relativePosition, lua_typename(L, valueType)).c_str());
            break;
        }
    }

    printf("%s\n", std::format(" --------- Stack Ends: {} ---------", stackName).c_str());
}

int lua::utils::traceback(lua_State* L) {
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
        if (str)
        {
            printf("%s\n", str);
        }
        else
        {
            printf("%s\n", luaL_typename(L, i));
        }

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
    return 1;
}

void lua::utils::PushAnyValueToStack(lua_State* L, CAnyValue* value)
{
    // number  -> Floating-point number (default)  | IEEE 754 double (64-bit)
    // integer -> Whole number (exact values only) | 64-bit signed integer

    switch(value->type)
    {
    case CType::C_ANY:
    case CType::C_INT_64:
    case CType::C_INT_32:
    case CType::C_INT_16:
    case CType::C_INT_8:
    case CType::C_UINT_64:
    case CType::C_UINT_32:
    case CType::C_UINT_16:
    case CType::C_UINT_8:
    {
        lua_pushinteger(L, ValueAs<int64_t>(value));
        break;
    }
    
    case CType::C_DOUBLE:
    case CType::C_FLOAT:
    {
        lua_pushnumber(L, ValueAs<double>(value));
        break;
    }

    case CType::C_STRING:
    {
        lua_pushstring(L, ValueAs<const char*>(value));
        break;
    }

    case CType::C_BOOL:
    {
        lua_pushboolean(L, ValueAs<bool>(value));
        break;
    }

    case CType::C_ARRAY:
    case CType::C_ANY_ARRAY:
    case CType::C_DICT:
    case CType::C_VECTOR:
    case CType::C_DATE: {
        printf("PushAnyValueToStack type (%d) is not supported", value->type);
        break;
    }

    default:
        printf("PushAnyValueToStack type (%d) not handled", value->type);
        break;
    }
}

CAnyValue* lua::utils::MakeAnyValue(AnyValueFactory* valueFactory, lua_State *L, int32_t index)
{
    switch (lua_type(L, index))
    {
    case lua_Type::LUA_TNIL:
        return valueFactory->MakeNullValue();

    case lua_Type::LUA_TBOOLEAN:
        return valueFactory->MakeBoolValue(lua_toboolean(L, index));

    case lua_Type::LUA_TNUMBER:
        return valueFactory->MakeDoubleValue(lua_tonumber(L, index));

    case lua_Type::LUA_TSTRING:
        return valueFactory->MakeStringValue(lua_tostring(L, index));

    default:
        break;
    }

    return nullptr;
}
