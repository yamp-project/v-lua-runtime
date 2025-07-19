#pragma once

#include <wrapper/type_traits.h>
#include <wrapper/values.h>
#include <wrapper/utils.h>

#include <lua.h>
#include <lualib.h>

#include <tuple>
#include <type_traits>

namespace lua
{
    struct Proxy
    {
        static int CApiClassIndex(lua_State* L);
        static int CApiClassTostring(lua_State* L);

        template<typename Class, typename... Args>
        static int CApiClassConstructor(lua_State* L)
        {
            typedef Class*(*Constructor)(Args...);
            constexpr size_t constructorArgumentSize = sizeof...(Args);

            const char* className = lua_tostring(L, lua_upvalueindex(1));
            Constructor constructor = (Constructor)lua_tolightuserdata(L, lua_upvalueindex(2));

            int stackSize = lua_gettop(L);
            int argumentsSize = stackSize - 1; //we skip the first value as it will be the table that this function will be called from

            if (argumentsSize != constructorArgumentSize)
            {
                printf("Arguments doesn't match!\n");
                lua_pushnil(L);
                return 1;
            }

            size_t argumentIndex = 2;
            Class* returnObject = constructor(Value<type_t<Args>>::Read(L, argumentIndex++).value()...);
            *static_cast<Class**>(lua_newuserdata(L, sizeof(Class*))) = returnObject;
            lua_getmetatable(L, 1);
            lua_setmetatable(L, -2);
            return 1;
        }

        template<typename ReturnType, typename Class, typename... Args>
        static int CApiClassMemberFunction(lua_State* L)
        {
            typedef ReturnType(*MemberFunction)(Class*, Args...);

            const char* className = lua_tostring(L, lua_upvalueindex(1));
            MemberFunction memberFunction = (MemberFunction)lua_tolightuserdata(L, lua_upvalueindex(2));

            Class* instance = *static_cast<Class**>(luaL_checkudata(L, 1, className));
            size_t argumentIndex = 0;

            if constexpr((sizeof...(Args) == 1) && std::is_same_v<std::tuple_element_t<0, std::tuple<Args...>>, lua_State*>)
            {
                memberFunction(instance, L);
                return 0;
            }
            else
            {
                if constexpr (!std::is_same_v<ReturnType, void>)
                {
                    Value<ReturnType>::Push(L, memberFunction(instance, Value<Args>::Read(L, (lua_gettop(L) - argumentIndex++)).value()...));
                }
                else
                {
                    memberFunction(instance, Value<Args>::Read(L, (lua_gettop(L) - argumentIndex++)).value()...);
                }
            }

            return 0;
        }

        template<typename ReturnType, typename Class>
        static int CApiClassMemberVariableGetter(lua_State* L)
        {
            typedef ReturnType(*GetterFunction)(Class*);

            const char* className = lua_tostring(L, lua_upvalueindex(1));
            GetterFunction getterFunction = (GetterFunction)lua_tolightuserdata(L, lua_upvalueindex(2));

            Class* instance = *static_cast<Class**>(luaL_checkudata(L, 1, className));
            Value<ReturnType>::Push(L, getterFunction(instance));

            return 1;
        }
    };
}