#pragma once

#include "definition.h"

namespace lua::Definitions
{
    class Native : public IDefinition
    {
    public:
        using IDefinition::IDefinition;

        // void Initialize() override;
        void Initialize() {};
    };
}