#ifndef SOURCE_LUAWRAPPER_H
#define SOURCE_LUAWRAPPER_H

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <typeindex>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>

#include <wrapper/utils.h>
#include <wrapper/proxy_functions.h>
#include <wrapper/values.h>
#include <wrapper/type_traits.h>

namespace lua
{
    class Namespace;

    template<typename T>
    class Class
    {
    public:
        Class(lua_State* state, std::string classMetaTableName, Namespace* parent) :
            m_State(state), m_ParentNamespace(parent), m_ClassMetaTableName(classMetaTableName)
        {

        };

        ~Class() = default;

        void GetName()
        {
            printf("GetName: %s\n", typeid(T).name());
        }

        template<typename ConstructorType>
        Class<T>& Constructor()
        {
            GetName();

            typename TypeTraits<ConstructorType>::class_type* asdasd = nullptr;
            printf("Asd: %s\n", typeid(TypeTraits<ConstructorType>::class_type).name());
            printf("Asd: %s\n", typeid(TypeTraits<ConstructorType>::arguments).name());
            printf("Hmmmm\n");

            return *this;
        }

        template<typename ClassType, typename... Args>
        Class<T>& Constructor(ClassType*(*function)(Args...))
        {
            //printf("Constructor: %s\n", typeid(CApiClassTraits<Function>::class_type).name());
            //printf("Constructor: %s\n", typeid(CApiClassTraits<Function>::arguments).name());
            //printf("Constructor: %s\n", typeid(CApiClassTraits<Function>::return_type).name());

            lua_pushstring(m_State, m_ClassMetaTableName.c_str());
            lua_pushlightuserdata(m_State, function);
            lua_pushcclosurek(m_State, Proxy::CApiClassConstructor<ClassType, Args...>, "Constructor", 2, NULL);
            lua_setfield(m_State, -2, "__call");

            Utils::lua_stacktrace(m_State, "Constructor");

            return *this;
        }

        template<typename ReturnType, typename ClassType, typename... Args>
        Class<T>& MemberFunction(std::string functionName, ReturnType(*function)(ClassType*, Args...))
        {
            /*static auto proxyFunction = [](lua_State* L) -> int
                {
                    printf("Proxy function\n");

                    return 0;
                };*/

            lua_pushstring(m_State, m_ClassMetaTableName.c_str());
            lua_pushlightuserdata(m_State, function);
            lua_pushcclosurek(m_State, Proxy::CApiClassMemberFunction<ReturnType, ClassType, Args...>, "MemberFunction", 2, NULL);
            lua_setfield(m_State, -2, functionName.c_str());

            Utils::lua_stacktrace(m_State, "MemberFunction");

            //m_Functions.push_back({ _strdup(functionName.c_str()), Proxy::CApiClassMemberFunction<Function> });

            return *this;
        }

        template<typename ReturnType, typename ClassType>
        Class<T>& MemberVariable(std::string variableName, ReturnType(*getter)(ClassType*), void(*setter)(ClassType*, ReturnType) = nullptr)
        {
            printf("MemberVariable!\n");

            lua_rawgetfield(m_State, -1, "__get");

            lua_pushstring(m_State, m_ClassMetaTableName.c_str());
            lua_pushlightuserdata(m_State, getter);
            lua_pushcclosurek(m_State, Proxy::CApiClassMemberVariableGetter<ReturnType, ClassType>, "__get", 2, NULL);
            lua_setfield(m_State, -2, variableName.c_str());

            lua_pop(m_State, 1);

            Utils::lua_stacktrace(m_State, "MemberVariable");

            return *this;
        }

        Namespace& EndClass()
        {
            Utils::lua_stacktrace(m_State, "EndClass");
            //m_Functions.push_back({ nullptr, nullptr });

            /*for (auto& s : m_Functions)
                printf("s: %s\n", s.name);*/

            //luaL_register(m_State, nullptr, m_Functions.data());
            Utils::lua_stacktrace(m_State, "EndClass2");

            //lua_pushvalue(m_State, -2);
            //lua_newtable(m_State);
            //lua_pushvalue(m_State, -1);
            //Utils::lua_stacktrace(m_State, "BeforeSetMetaTable");
            lua_setmetatable(m_State, -2);
            ////namespace is on the stack
            /*if (lua_istable(m_State, -2))
            {
                lua_setfield(m_State, -2, );
            }*/

            if (lua_istable(m_State, -3))
            {
                lua_rawset(m_State, -3);
            }

            Utils::lua_stacktrace(m_State, "AfterSetMetaTable");
            printf("End the class!\n");

            return *m_ParentNamespace;
        }

    private:
        Namespace* m_ParentNamespace;
        lua_State* m_State;
        std::string m_ClassMetaTableName;

        //std::vector<luaL_Reg> m_Functions;
    };

    class Namespace
    {
    public:
        Namespace(lua_State* state) :
            m_State(state)
        {
            //lua_gettable(m_State, LUA_GLOBALSINDEX);
            lua_getglobal(m_State, "_G");
            m_Name = "global";
            m_IsGlobalNamespace = true;
        }

        Namespace(lua_State* state, std::string namespaceName, Namespace* parent) :
            m_State(state)
        {
            if (lua_istable(m_State, -1))
            {
                lua_getfield(m_State, -1, namespaceName.c_str());
                
                printf("Hmmm\n");

                //if namespace was already registered the result will be on top of the stack
                if (lua_istable(m_State, -1))
                    return;

                //pop the nil result and create an empty table
                lua_pop(m_State, 1);
                lua_newtable(m_State);

                m_Name = namespaceName;
                m_ParentNamespace = parent;
            }
        }

        ~Namespace()
        {
            if (m_IsGlobalNamespace)
                lua_pop(m_State, 1);

            //EndNamespace();
            printf("Namespace destructor %s\n", m_Name.c_str());
        }

        Namespace BeginNamespace(std::string namespaceName)
        {
            /*lua_getfield(m_State, LUA_REGISTRYINDEX, namespaceName);
            if (lua_istable(m_State, -1))
                return *this;

            lua_newtable(m_State);
            lua_pushvalue(m_State, -1);
            lua_pushvalue(m_State, -1);

            Utils::lua_stacktrace(m_State, "GetNamespace");

            lua_setfield(m_State, LUA_REGISTRYINDEX, namespaceName);
            lua_setglobal(m_State, namespaceName);*/

            printf("BeginNamespace: %s\n", namespaceName.c_str());

            return Namespace(m_State, namespaceName, this);
        }

        Namespace& EndNamespace()
        {
            //Utils::lua_stacktrace(m_State, "EndNamespace");

            if (lua_istable(m_State, -2) && !m_IsGlobalNamespace)
            {
                lua_setfield(m_State, -2, m_Name.c_str());
                printf("TOP IS A TABLE: %s\n", m_Name.c_str());
            }

            //Utils::lua_stacktrace(m_State, "EndNamespace2");
            return *m_ParentNamespace;
        }

        template<typename T>
        Namespace& RegisterVariable(std::string key, T value)
        {
            Value<T>::Push(m_State, value);
            //lua_setglobal(m_State, key.c_str());

            if (lua_istable(m_State, -2))
            {
                lua_setfield(m_State, -2, key.c_str());
            }

            return *this;
        }

        template<typename T>
        Namespace& RegisterFunction(std::string functionName, T function)
        {
            Value<T>::Push(m_State, function);
            lua_setglobal(m_State, functionName.c_str());

            printf("Registering function: %s\n", functionName.c_str());

            return *this;
        }

        template<typename T>
        Class<T> BeginClass(std::string className)
        {
            Utils::lua_stacktrace(m_State, "BeginClass");

            lua_pushstring(m_State, className.c_str());
            lua_newtable(m_State);

            if (luaL_newmetatable(m_State, className.c_str()))
            {
                //lua_pushvalue(m_State, -1);
                //lua_pop(m_State, 1);
                lua_pushcfunction(m_State, Proxy::CApiClassIndex, "__index");
                lua_setfield(m_State, -2, "__index");

                lua_newtable(m_State);
                lua_setfield(m_State, -2, "__set");

                lua_newtable(m_State);
                lua_setfield(m_State, -2, "__get");
            }

            //lua_pushstring(m_State, className);
            //if (luaL_newmetatable(m_State, typeid(T).name()))
            //{
            //    printf("Creating the metatable\n");
            //    Utils::lua_stacktrace(m_State, "BeginClass");

            //    //push the metatable to the stack again as lua_setfield will pop it right away (nah, not true)
            //    //lua_pushvalue(m_State, -1);

            /*static auto a = [](lua_State* L) -> int
                {
                    printf("__call!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

                    return 0;
                };*/

            //lua_pushcfunction(m_State, a, "BeginClass::Call");
            //lua_setfield(m_State, -2, "__call");

            //lua_setmetatable(m_State, -2);

            Utils::lua_stacktrace(m_State, "BeginClass2");

            //

            //    Utils::lua_stacktrace(m_State, "BeginClass2");
            //}

            if (!m_RegisteredClasses.contains(typeid(T)))
                m_RegisteredClasses[typeid(T)] = className;

            return Class<T>(m_State, className, this);
        }

    private:

        /*~Namespace()
        {
            printf("Namespace destructor\n");
        }*/

        Namespace* m_ParentNamespace = nullptr;
        bool m_IsGlobalNamespace = false;
        std::string m_Name = "";
        lua_State* m_State = nullptr;

        std::unordered_map<std::type_index, std::string> m_RegisteredClasses;
    };

    class State
    {
    public:
        State();
        ~State();

        void RunFile(std::string filePath);

        Namespace GetGlobalNamespace()
        {
            printf("state: %p\n", m_State);
            return Namespace(m_State);
        }

        //State& GetNamespace(const char* namespaceName)
        //{
        //    lua_getfield(m_State, LUA_REGISTRYINDEX, namespaceName);
        //    if (!lua_isnil(m_State, -1))
        //        return *this;

        //    lua_newtable(m_State);
        //    lua_pushvalue(m_State, -1);
        //    lua_pushvalue(m_State, -1);

        //    Utils::lua_stacktrace(m_State, "GetNamespace");

        //    lua_setfield(m_State, LUA_REGISTRYINDEX, namespaceName);
        //    lua_setglobal(m_State, namespaceName);

        //    return *this;
        //}

        lua_State* GetState() { return m_State; }

    private:
        lua_State* m_State = nullptr;
    };
}

#endif //SOURCE_LUAWRAPPER_H
