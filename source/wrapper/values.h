#pragma once

#include <optional>
#include <string>

#include <lua.h>

#include <wrapper/type_traits.h>
#include <wrapper/utils.h>

#define CHECK_TYPE_AND_INDEX(state, type, index) \
    if(index > lua_gettop(state)) return std::nullopt; \
    if (lua_type(state, index) != type) return std::nullopt

namespace lua
{
    struct FunctionRef
    {
        uint32_t m_Reference;
        const void* m_Pointer;
    };

    template<typename T>
    struct Value {};

    template<>
    struct Value<void>
    {
        static inline std::optional<void*> Read(lua_State* state, int index)
        {
            return std::nullopt;
        };

        static inline void Push(lua_State* state, void* a = nullptr)
        {
            lua_pushnil(state);
        };
    };

    template<typename Integer> requires std::is_integral<Integer>::value
    struct Value<Integer>
    {
        static inline std::optional<Integer> Read(lua_State* state, int index)
        {
            CHECK_TYPE_AND_INDEX(state, LUA_TNUMBER, index);

            int temp;
            Integer result = static_cast<Integer>(lua_tointegerx(state, index, &temp));

            printf("Value<Integer>::Read3: %d\n", index);

            return (temp == 1) ? std::make_optional(result) : std::nullopt;
        }

        static inline void Push(lua_State* state, Integer value)
        {
            lua_pushinteger(state, value);
        }
    };

    template<typename Float> requires std::is_floating_point<Float>::value
    struct Value<Float>
    {
        static inline std::optional<Float> Read(lua_State* state, int index)
        {
            CHECK_TYPE_AND_INDEX(state, LUA_TNUMBER, index);

            int temp;
            Float result = static_cast<Float>(lua_tonumberx(state, index, &temp));

            return (temp == 1) ? std::make_optional(result) : std::nullopt;
        }

        static inline void Push(lua_State* state, Float value)
        {
            lua_pushnumber(state, value);
        }
    };

    template<>
    struct Value<const char*>
    {
        static inline std::optional<const char*> Read(lua_State* state, int index)
        {
            CHECK_TYPE_AND_INDEX(state, LUA_TSTRING, index);

            const auto value = lua_tostring(state, index);
            if (value == NULL)
                return std::nullopt;

            return value;
        }

        static inline void Push(lua_State* state, const char* value)
        {
            lua_pushstring(state, value);
        }
    };

    template<>
    struct Value<std::string>
    {
        static inline std::optional<std::string> Read(lua_State* state, int index)
        {
            return Value<const char*>::Read(state, index);
        }

        static inline void Push(lua_State* state, const std::string& value)
        {
            lua_pushstring(state, value.c_str());
        }
    };

    template<>
    struct Value<bool>
    {
        static inline std::optional<bool> Read(lua_State* state, int index)
        {
            CHECK_TYPE_AND_INDEX(state, LUA_TNUMBER, index);

            return lua_toboolean(state, index);
        }

        static inline void Push(lua_State* state, bool value)
        {
            lua_pushboolean(state, value);
        }
    };

    template<>
    struct Value<FunctionRef>
    {
        static inline std::optional<FunctionRef> Read(lua_State* state, int index)
        {
            CHECK_TYPE_AND_INDEX(state, LUA_TFUNCTION, index);

            FunctionRef tmp;
            tmp.m_Reference = lua_ref(state, index);
            tmp.m_Pointer = lua_topointer(state, index);

            return tmp;
        }

        static inline void Push(lua_State* state, FunctionRef value)
        {
            printf("Push FunctionRef\n");
        }
    };

    template<>
    struct Value<lua_CFunction>
    {
        static inline void Read(lua_State* state, int index)
        {
            //printf("Read function\n");
        }

        static inline void Push(lua_State* state, lua_CFunction value)
        {
            lua_pushcfunction(state, value, "Push::lua_CFunction");
            //printf("Push function\n");
            //lua_pushlightuserdata(state, value);
            //lua_pushcclosure(state, Proxy, "Value<function>", 1);
        }
    };

    template<typename ReturnType, typename... Args>
    struct Value<ReturnType(*)(Args...)>
    {
        static inline int Proxy(lua_State* state)
        {
            typedef ReturnType(*OriginalFunction__Type)(Args...);
            OriginalFunction__Type realFunction = reinterpret_cast<OriginalFunction__Type>(lua_touserdata(state, lua_upvalueindex(1)));

            int index = 0;
            realFunction(Value<type_t<Args>>::Read(state, sizeof...(Args) - index++)...);

            //printf("Proxy function has been called!\n");
            return 0;
        }

        static inline void Read(lua_State* state, int index)
        {
            printf("Read function\n");
        }

        static inline void Push(lua_State* state, ReturnType(*value)(Args...))
        {
            printf("Push function\n");
            lua_pushlightuserdata(state, value);
            lua_pushcclosure(state, Proxy, "Value<function>", 1);
        }
    };
}