#pragma once

#include <yamp-sdk/lookup_table.h>

namespace lua
{
    class State;
    class Resource;
}

namespace lua::Definitions
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
}