#pragma once

#include <vector>
#include <functional>

#include <yamp-sdk/lookup_table.h>

namespace lua
{
    class State;
    class Resource;
}

namespace lua
{
    class StaticDefinition
    {
    public:
        typedef std::function<void(Resource*)> DefinitionCallback;

        StaticDefinition(DefinitionCallback callback)
        {
            m_Definitions.push_back(this);
            m_Callback = callback;
        }

        inline static std::vector<StaticDefinition*> m_Definitions;

        DefinitionCallback m_Callback = nullptr;
    };
}