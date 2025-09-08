#include "resource.h"
#include "runtime.h"

#include "wrapper/utils.h"
#include "wrapper/lua_wrapper.h"

#include <filesystem>
#include <iostream>
#include <fstream>

namespace lua
{
    void CallCoreEvent(lua::Resource* resource, CoreEventType eventType, CAnyArray* args) {
        lua_State* L = resource->GetLuaState();

        auto refs = resource->GetCoreCallbackRef(eventType);
        if (refs)
        {
            for (int32_t ref : *refs)
            {
                lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
                for (size_t i = 0; i < args->size; ++i)
                {
                    lua::utils::PushAnyValueToStack(L, args->buffer[i]);
                }

                if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                    printf("Error calling Lua callback: %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
    }

    void CallResourceEvent(lua::Resource* resource, std::string_view eventName) {
        lua_State* L = resource->GetLuaState();

        auto refs = resource->GetCallbackRef(eventName);
        if (refs)
        {
            for (int32_t ref : *refs)
            {
                lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

                lua_createtable(L, 1, 2);
                lua_pushstring(L, "value");
                lua_setfield(L, -2, "key");

                lua_pushstring(L, "u42");
                lua_setfield(L, -2, "__type");

                if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                    printf("Error calling Lua callback: %s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
            }
        }
    }

    Resource::Resource(SDK_Interface* sdk, SDK_Resource* sdkResource) :
        m_Logger(Logger(sdk, std::format("resource {}", sdkResource->name))),
        m_State(State(sdkResource->name)),
        m_Resource(sdkResource)
    {
        //RegisterDefinition<lua::Definitions::Native>();
    }

    void Resource::RegisterCallbackRef(std::string_view identifier, int32_t ref)
    {
        m_CallbackRefs[std::string(identifier)].push_back(ref);
    }

    std::vector<int32_t>* Resource::GetCallbackRef(std::string_view identifier)
    {
        auto it = m_CallbackRefs.find(identifier.data());
        if (it == m_CallbackRefs.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    void Resource::RegisterCoreCallbackRef(int32_t identifier, int32_t ref)
    {
        m_CoreCallbackRefs[identifier].push_back(ref);
    }

    std::vector<int32_t>* Resource::GetCoreCallbackRef(int32_t identifier)
    {
        auto it = m_CoreCallbackRefs.find(identifier);
        if (it == m_CoreCallbackRefs.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    void Resource::OnStart()
    {
        std::filesystem::path resourcePath = m_Resource->path;
        std::filesystem::path mainFilePath = resourcePath / m_Resource->mainFile;
    
        m_Logger.Info("Main file path: %s", mainFilePath.string().c_str());

        if (!std::filesystem::exists(mainFilePath))
        {
            return;
        }

        for (auto& definition : StaticDefinition::m_Definitions)
        {
            definition->m_Callback(this);
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

    void Resource::OnCoreEvent(CoreEventType eventType, CAnyArray* args)
    {
        CallCoreEvent(this, eventType, args);
    }

    void Resource::OnResourceEvent(const char* eventName, CAnyArray* args)
    {
        CallResourceEvent(this, eventName);
    }
} // namespace lua
