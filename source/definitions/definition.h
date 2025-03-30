#pragma once

#include <c-sdk/lookup_table.h>

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

    protected:
        IDefinition(lua::State* state);

        lua::State* m_State = nullptr;
        SdkLookupTable* m_LookupTable = nullptr;
    };
}