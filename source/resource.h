#pragma once

extern "C"
{
    #include <yamp-sdk/sdk.h>
};

#include "definitions/definition.h"
#include "wrapper/lua_wrapper.h"
#include "logger.h"

#include <vector>

namespace lua
{
    class Resource
    {
    public:
        void OnStart();
        void OnStop();
        void OnTick();
        void OnEvent();

        template<typename T> requires std::is_base_of_v<lua::Definitions::IDefinition, T>
        void RegisterDefinition()
        {
            m_Definitions.emplace_back(std::make_unique<T>(&m_State));
        }

        Resource(ILookupTable* lookupTable, IResource* resource);
        ~Resource() = default;

    private:
        IResource* m_Resource;
        Logger m_Logger;
        State m_State;

        std::vector<std::unique_ptr<Definitions::IDefinition>> m_Definitions;
    };
}
