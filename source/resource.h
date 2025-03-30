#ifndef SOURCE_RESOURCE_H
#define SOURCE_RESOURCE_H

#include "definitions/definition.h"

#include <filesystem>
#include <string>
#include <vector>

extern "C"
{
#include <c-sdk/resource.h>
};

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
            m_Definitions.push_back(new T(m_State));
        }

    private:
        Resource(SdkLookupTable* lookupTable, SdkResource *resource);
        ~Resource() = default;

        lua::State* m_State = nullptr;
        lua::Logger* m_Logger = nullptr;
        Runtime* m_Runtime = nullptr;

        SdkLookupTable* m_LookupTable = nullptr;
        SdkResource* m_Resource = nullptr;

        std::vector<lua::Definitions::IDefinition*> m_Definitions;
    };
}

#endif //SOURCE_RESOURCE_H
