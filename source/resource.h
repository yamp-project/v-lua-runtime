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

        ILookupTable* GetLookupTable()
        {
            return m_LookupTable;
        }

        State* GetState()
        {
            return &m_State;
        }

        lua_State* GetLuaState()
        {
            return m_State.GetState();
        }

        Resource(ILookupTable* lookupTable, IResource* resource);
        ~Resource() = default;

        IResource* m_Resource;

    private:
        ILookupTable* m_LookupTable;
        Logger m_Logger;
        State m_State;

        std::unordered_map<std::string, std::vector<int32_t>> m_CallbackRefs;
        std::unordered_map<int32_t, std::vector<int32_t>> m_CoreCallbackRefs;
    };
}
