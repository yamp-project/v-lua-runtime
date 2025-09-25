#ifndef SOURCE_LUAWRAPPER_H
#define SOURCE_LUAWRAPPER_H

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
//#include <typeindex>
#include <queue>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>


#include <wrapper/utils.h>
#include <wrapper/proxy_functions.h>
#include <wrapper/values.h>
#include <wrapper/type_traits.h>

#include "logger.h"

namespace lua
{
    class State
    {
    public:
        State() : State("") { }
        State(std::string resourceName);
        ~State();

        void RunFile(std::string filePath, std::string relativePath = "./");

        void GetGlobalNamespace()
        {
            m_NamespaceQueue.push_back("global");
            lua_getglobal(m_State, "_G");
        }

        void BeginClass(std::string className)
        {
            lua_pushstring(m_State, className.c_str());
            lua_newtable(m_State);

            if (luaL_newmetatable(m_State, className.c_str()))
            {
                //lua_pushvalue(m_State, -1);
                //lua_pop(m_State, 1);
                lua_pushcfunction(m_State, Proxy::CApiClassIndex, "__index");
                lua_setfield(m_State, -2, "__index");

                lua_pushcfunction(m_State, Proxy::CApiClassNewIndex, "__newindex");
                lua_setfield(m_State, -2, "__newindex");

                lua_pushcfunction(m_State, Proxy::CApiClassTostring, "__tostring");
                lua_setfield(m_State, -2, "__tostring");

                lua_newtable(m_State);
                lua_setfield(m_State, -2, "__set");

                lua_newtable(m_State);
                lua_setfield(m_State, -2, "__get");

                lua_pushstring(m_State, className.c_str());
                lua_setfield(m_State, -2, "__name");
            }

            m_ClassMetaTableQueue.push_back(className);
        }

        template<typename ClassType, typename... Args>
        void Constructor(ClassType*(*function)(Args...))
        {
            std::string& classMetaTableName = m_ClassMetaTableQueue.back();
            lua_pushstring(m_State, classMetaTableName.c_str());
            lua_pushlightuserdata(m_State, function);
            lua_pushcclosurek(m_State, Proxy::CApiClassConstructor<ClassType, Args...>, "Constructor", 2, NULL);
            lua_setfield(m_State, -2, "__call");

            //return *this;
        }

        template<typename ReturnType, typename ClassType>
        void Getter(std::string variableName, ReturnType(*getter)(ClassType*)) {
            lua_rawgetfield(m_State, -1, "__get");

            std::string& classMetaTableName =  m_ClassMetaTableQueue.back();
            lua_pushstring(m_State, classMetaTableName.c_str());
            lua_pushlightuserdata(m_State, getter);
            lua_pushcclosurek(m_State, Proxy::CApiClassMemberVariableGetter<ReturnType, ClassType>, "__get", 2, NULL);
            lua_setfield(m_State, -2, variableName.c_str());

            lua_pop(m_State, 1);
        }

        template<typename ReturnType, typename ClassType>
        void Setter(std::string variableName, void(*setter)(ClassType*, ReturnType))
        {
            lua_rawgetfield(m_State, -1, "__set");

            std::string& classMetaTableName =  m_ClassMetaTableQueue.back();
            lua_pushstring(m_State, classMetaTableName.c_str());
            lua_pushlightuserdata(m_State, setter);
            lua_pushcclosurek(m_State, Proxy::CApiClassMemberVariableSetter<ReturnType, ClassType>, "__get", 2, NULL);
            lua_setfield(m_State, -2, variableName.c_str());

            lua_pop(m_State, 1);
        }

        template<typename ReturnType, typename ClassType, typename... Args>
        void MemberFunction(std::string functionName, ReturnType(*function)(ClassType*, Args...))
        {
            std::string& classMetaTableName = m_ClassMetaTableQueue.back();
            lua_pushstring(m_State, classMetaTableName.c_str());
            lua_pushlightuserdata(m_State, function);
            lua_pushcclosurek(m_State, Proxy::CApiClassMemberFunction<ReturnType, ClassType, Args...>, _strdup(functionName.c_str()), 2, NULL);
            lua_setfield(m_State, -2, functionName.c_str());
        }

        void EndClass()
        {
            lua_setmetatable(m_State, -2);

            if (lua_istable(m_State, -3))
            {
                lua_rawset(m_State, -3);
            }

            if (!m_ClassMetaTableQueue.empty()) {
                m_ClassMetaTableQueue.pop_back();
            }
        }

        void BeginNamespace(std::string namespaceName)
        {
            if (lua_istable(m_State, -1))
            {
                lua_getfield(m_State, -1, namespaceName.c_str());

                // if namespace was already registered the result will be on top of the stack
                if (lua_istable(m_State, -1))
                    return;

                // pop the nil result and create an empty table
                lua_pop(m_State, 1);
                lua_newtable(m_State);

                m_NamespaceQueue.push_back(namespaceName);
            }
        }

        void EndNamespace()
        {
            // handle global namespace is on the stack (hopefully)
            if(lua_gettop(m_State) == 1)
            {
                lua_pop(m_State, 1);
            }

            if(lua_gettop(m_State) >= 2)
            {
                if (!m_NamespaceQueue.empty()) {
                    std::string currentNamespace = m_NamespaceQueue.back();
                    lua_setfield(m_State, -2, currentNamespace.c_str());
                }
            }

            if (!m_NamespaceQueue.empty()) {
                m_NamespaceQueue.pop_back();
            }
        }

        void RegisterVariable(std::string key)
        {
            assert(lua_gettop(m_State) >= 2 && "Not enough value pushed onto the stack to register a variable!");
            if (lua_istable(m_State, -2))
            {
                lua_setfield(m_State, -2, key.c_str());
            }
        }

        template<typename T>
        void RegisterVariable(std::string key, T value)
        {
            Push(value);

            assert(lua_gettop(m_State) >= 2 && "Not enough value pushed onto the stack to register a variable!");
            if (lua_istable(m_State, -2))
            {
                lua_setfield(m_State, -2, key.c_str());
            }
        }

        void RegisterCFunction(std::string functionName, lua_CFunction function, int closureNup = 0)
        {
            lua_pushcclosurek(m_State, function, "", closureNup, NULL);

            assert(lua_gettop(m_State) >= 2 && "Not enough value pushed onto the stack to register a function!");
            if (lua_istable(m_State, -2)) {
                //lua_setglobal(m_State, functionName.c_str());
                lua_setfield(m_State, -2, functionName.c_str());
            }
        }

        template<typename T>
        void RegisterFunction(std::string functionName, T function)
        {
            Push(function);

            assert(lua_gettop(m_State) >= 2 && "Not enough value pushed onto the stack to register a function!");
            if (lua_istable(m_State, -2)) {
                //lua_setglobal(m_State, functionName.c_str());
                lua_setfield(m_State, -2, functionName.c_str());
            }

            //return *this;
        }

        lua_State* GetState() { return m_State; }

        template<typename T>
        void Push(T value)
        {
            Value<T>::Push(m_State, std::forward<T>(value));
        }

        template<typename T>
        void PushObject(T value, std::string className)
        {
            //lua_pushlightuserdata(m_State, value);
            *static_cast<T*>(lua_newuserdata(m_State, sizeof(void*))) = value;
            luaL_getmetatable(m_State, className.c_str());

            if (lua_isnil(m_State, -1))
                return;

            //utils::lua_stacktrace(m_State, "PushObject");
            lua_setmetatable(m_State, -2);
            //utils::lua_stacktrace(m_State, "PushObject2");
        }

    private:
        std::string m_ResourceName = "";
        lua_State* m_State = nullptr;
        // lua::Logger* m_Logger = nullptr;

        std::deque<std::string> m_NamespaceQueue;
        std::deque<std::string> m_ClassMetaTableQueue;
    };
}

#endif //SOURCE_LUAWRAPPER_H
