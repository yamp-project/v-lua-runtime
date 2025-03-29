#pragma once

#include <wrapper/type_traits.h>
#include <wrapper/values.h>
#include <wrapper/utils.h>

#include <lua.h>
#include <lualib.h>

namespace lua
{
    struct Proxy
    {
        static int CApiClassIndex(lua_State* L);

        template<typename Class, typename... Args>
        static int CApiClassConstructor(lua_State* L)
        {
            typedef Class*(*Constructor)(Args...);
            constexpr size_t constructorArgumentSize = sizeof...(Args);

            const char* className = lua_tostring(L, lua_upvalueindex(1));
            Constructor constructor = (Constructor)lua_tolightuserdata(L, lua_upvalueindex(2));

            printf("CApiClassConstructor: %s\n", className);

            int stackSize = lua_gettop(L);
            int argumentsSize = stackSize - 1; //we skip the first value as it will be the table that this function will be called from

            if (argumentsSize != constructorArgumentSize)
            {
                printf("Arguments doesn't match!\n");
                lua_pushnil(L);
                return 1;
            }


            /*ReturnType returnValue;
            if constexpr (constructorArgumentSize == 0)
                returnValue = constructor();
            else
            {
                constexpr int index = 0;
                returnValue = constructor(Value<std::get<index>(Arguments)>::Read(L, constructorArgumentSize - index++)...);
            }*/
            
            printf("Arg shit: %d\n", sizeof...(Args));

            //lua_getmetatable(L, 1);

            /*luaL_getmetatable(L, className);
            if (!lua_istable(L, -1))
            {
                printf("Ohh ohhh something wrong happened!\n");
                lua_pushnil(L);
                return 1;
            }*/

            size_t argumentIndex = 2;
            Class* returnObject = constructor(Value<type_t<Args>>::Read(L, argumentIndex++).value()...);
            *static_cast<Class**>(lua_newuserdata(L, sizeof(Class*))) = returnObject;
            lua_getmetatable(L, 1);
            lua_setmetatable(L, -2);

            Utils::lua_stacktrace(L, "CApiClassConstructor");
            printf("Class type: %s\n", typeid(Class).name());
            printf("Class shit: %p\n", returnObject);
            //lua_pop(L, 1);

            //printf("Constructor: %s - %d\n", typeid(Arguments).name(), std::tuple_size_v<Arguments>);
            //printf("Constructor: %s\n", typeid(ReturnType).name());

            return 1;
        }

        template<typename ReturnType, typename Class, typename... Args>
        static int CApiClassMemberFunction(lua_State* L)
        {
            typedef ReturnType(*MemberFunction)(Class*, Args...);

            const char* className = lua_tostring(L, lua_upvalueindex(1));
            MemberFunction memberFunction = (MemberFunction)lua_tolightuserdata(L, lua_upvalueindex(2));

            Class* instance = *static_cast<Class**>(luaL_checkudata(L, 1, className));
            size_t argumentIndex = 2;

            if constexpr (!std::is_same_v<ReturnType, void>)
            {
                Value<ReturnType>::Push(L, memberFunction(instance, Value<type_t<Args>>::Read(L, argumentIndex++).value()...));
            }
            else
            {
                memberFunction(instance, Value<type_t<Args>>::Read(L, argumentIndex++).value()...);
            }
            //Value<ReturnType>::Push(L, memberFunction(instance, Value<type_t<Args>>::Read(L, argumentIndex++).value()...));



            Utils::lua_stacktrace(L, "CApiClassMemberFunction");

            printf("Member function has been called!\n");
            printf("Return type: %s\n", typeid(ReturnType).name());
            /*printf("Before checkudata\n");
            luaL_checkudata(L, 1, className);
            printf("After checkudata\n");

            printf("Class name: %s\n", className);
            printf("CApiClassMemberFunction: %s\n", typeid(CApiClassTraits<Function>::class_type).name());
            printf("CApiClassMemberFunction: %s\n", typeid(CApiClassTraits<Function>::arguments).name());
            printf("CApiClassMemberFunction: %s\n", typeid(CApiClassTraits<Function>::return_type).name());
            printf("Proxy has been called!!!! %p\n", function);*/

            return 0;
        }

        template<typename ReturnType, typename Class>
        static int CApiClassMemberVariableGetter(lua_State* L)
        {
            typedef ReturnType(*GetterFunction)(Class*);

            const char* className = lua_tostring(L, lua_upvalueindex(1));
            GetterFunction getterFunction = (GetterFunction)lua_tolightuserdata(L, lua_upvalueindex(2));

            Class* instance = *static_cast<Class**>(luaL_checkudata(L, 1, className));



            Utils::lua_stacktrace(L, "CApiClassMemberVariableGetter");

            return 1;
        }
    };
}