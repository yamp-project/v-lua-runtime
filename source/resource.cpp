#include "resource.h"
#include "runtime.h"

#include "wrapper/utils.h"
#include "wrapper/lua_wrapper.h"
#include "definitions/native.h"

#include <filesystem>
#include <iostream>
#include <fstream>

int luaL_ref(lua_State* L, int t)
{
    assert(t == LUA_REGISTRYINDEX);
    int r = lua_ref(L, -1);
    lua_pop(L, 1);
    return r;
}

namespace lua
{
    int RegisterEvent(lua_State* L) {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Expected an event name");
            return 1;
        }

        if (!lua_isfunction(L, 2)) {
            luaL_error(L, "Expected a function");
            return 1;
        }

        Resource* resource = Runtime::GetInstance()->GetStateResource(L);
        resource->RegisterCallbackRef(lua_tostring(L, 1), luaL_ref(L, LUA_REGISTRYINDEX));

        return 0;
    }

    void CallLuaEvent(Resource* resource, CoreEvent& event) {
        lua_State* L = resource->GetState();

        // TODO: maybe the callback identifier should not be strings
        // either way make it depend on the actual core event, and maybe
        // "events" in general
        auto refs =  resource->GetCallbackRef("weaponFire");
        if (refs)
        {
            for (int32_t ref : *refs)
            {
                lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
                for (size_t i = 0; i < event.args->size; ++i)
                {
                    utils::PushAnyValueToStack(L, event.args->buffer[i]);
                }

                if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                    std::cerr << "Error calling Lua callback: " << lua_tostring(L, -1) << "\n";
                    lua_pop(L, 1);
                }
            }
        }
    }

    int l_my_print(lua_State* L)
    {
        Logger* logger = Runtime::GetInstance()->GetLogger();
        std::string strBuilder = "";
    
        int nargs = lua_gettop(L);
        for (int i = 1; i <= nargs; ++i)
        {
            strBuilder.append(std::format("{}{}", (i == 1 ? "" : " "), luaL_tolstring(L, i, nullptr)));
            lua_pop(L, 1);
        }

        logger->Info(strBuilder.c_str());
        return 0;
    }

    Resource::Resource(ILookupTable* lookupTable, IResource* resource) :
        m_Logger(Logger(lookupTable, std::format("resource {}", resource->name))),
        m_State(State(resource->name)),
        m_Resource(resource)
    {
        RegisterDefinition<lua::Definitions::Native>();
    }

    void Resource::RegisterCallbackRef(std::string_view identifier, int32_t ref)
    {
        m_CallbackRefs[identifier.data()].push_back(ref);
    }

    std::vector<int32_t>* Resource::GetCallbackRef(std::string_view identifier)
    {
        if (!m_CallbackRefs.contains(identifier.data()))
        {
            return nullptr;
        }

        return &m_CallbackRefs[identifier.data()];
    }

    void Resource::OnStart()
    {
        std::filesystem::path resourcePath = m_Resource->resourcePath;
        std::filesystem::path mainFilePath = resourcePath / m_Resource->resourceMainFile;
    
        m_Logger.Info("Main file path: %s", mainFilePath.string().c_str());

        if (!std::filesystem::exists(mainFilePath))
        {
            return;
        }

        m_State.GetGlobalNamespace();
        m_State.RegisterCFunction("print", l_my_print);
            m_State.BeginNamespace("yamp");
                m_State.RegisterCFunction("on", RegisterEvent);
            m_State.EndNamespace();
        m_State.EndNamespace();

        for(auto& definition : m_Definitions)
        {
            definition->Initialize();
        }

        m_Logger.Info("Running the main file: %s", mainFilePath.string().c_str());
        m_State.RunFile(mainFilePath.string(), resourcePath.string());
    }

    void Resource::OnStop()
    {
        //
    }

    void Resource::OnTick()
    {
        //
    }

    void Resource::OnEvent(CoreEvent& event)
    {
        CallLuaEvent(this, event);
    }
} // namespace lua
