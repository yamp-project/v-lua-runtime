#pragma once

#include <vector>

extern "C"
{
    #include <yamp-sdk/sdk.h>
};

#include "definitions/definition.h"
#include "logger.h"

namespace lua
{
    class State;
    class Runtime;

    class Resource
    {
        friend class Runtime;

    public:
        void OnStart();
        void OnStop();
        void OnTick();

        template<typename T> requires std::is_base_of_v<lua::Definitions::IDefinition, T>
        void RegisterDefinition()
        {
            // TODO: remove this raw heap allocation
            m_Definitions.push_back(new T(m_State.get()));
        }

        Resource(ILookupTable* lookupTable, IResource *resource);

    private:
        ~Resource() = default;
        IResource* m_Resource;

        std::unique_ptr<Logger> m_Logger;
        std::unique_ptr<State> m_State;
        std::vector<Definitions::IDefinition*> m_Definitions;
    };
}
