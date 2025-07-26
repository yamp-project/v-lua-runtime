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
    class IDefinition
    {
        friend class lua::Resource;

    public:
        virtual void Initialize() = 0;

        IDefinition(lua::State* state);
    
    protected:
        lua::State* m_State = nullptr;
        ILookupTable* m_LookupTable = nullptr;
    };

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