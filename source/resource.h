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

        void OnStart();
        void OnStop();
        void OnTick();
        void OnEvent(CoreEvent& event);

        template<typename T> requires std::is_base_of_v<lua::Definitions::IDefinition, T>
        void RegisterDefinition()
        {
            m_Definitions.emplace_back(std::make_unique<T>(&m_State));
        }

        lua_State* GetState()
        {
            return m_State.GetState();
        }

        Resource(ILookupTable* lookupTable, IResource* resource);
        ~Resource() = default;

    private:
        IResource* m_Resource;
        Logger m_Logger;
        State m_State;

        std::vector<std::unique_ptr<Definitions::IDefinition>> m_Definitions;
        std::unordered_map<std::string, std::vector<int32_t>> m_CallbackRefs;
    };
}
