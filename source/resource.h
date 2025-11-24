#pragma once

#include "definitions/definition.h"
#include "wrapper/lua_wrapper.h"

// TODO: forward declare
#include <yamp-sdk/sdk.h>
#include <vector>

namespace lua
{
    class Resource
    {
    public:
        void RegisterCallbackRef(std::string_view identifier, int32_t ref);
        std::vector<int32_t>* GetCallbackRef(std::string_view identifier);

        void RegisterCoreCallbackRef(int32_t identifier, int32_t ref);
        std::vector<int32_t>* GetCoreCallbackRef(int32_t identifier);

        void OnStart();
        void OnStop();
        void OnTick();
        void OnCoreEvent(CoreEventType eventType, CAnyArray* args);
        void OnResourceEvent(const char* eventName, CAnyArray* args);

        SDK_ResourceStatistic* GetStatistic();

        State* GetState()
        {
            return &m_State;
        }

        lua_State* GetLuaState()
        {
            return m_State.GetState();
        }

        Resource(SDK_Interface* sdk, SDK_Resource* sdkResource);
        ~Resource() = default;

        std::unique_ptr<SDK_Resource> m_Resource;
    private:
        Logger m_Logger;
        State m_State;
        SDK_ResourceStatistic m_ResourceStatistic;

        std::unordered_map<std::string, std::vector<int32_t>> m_CallbackRefs;
        std::unordered_map<int32_t, std::vector<int32_t>> m_CoreCallbackRefs;
    };
}
