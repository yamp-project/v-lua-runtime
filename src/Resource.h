#ifndef _LUA_RESOURCE_H
#define _LUA_RESOURCE_H

#include <v-sdk/Resource.hpp>

namespace lua
{
    namespace sdk = yamp::sdk;
    class Resource : public sdk::IResourceBase
    {
    public:
        Resource(sdk::ResourceInformation* information);
        virtual ~Resource();

        sdk::ResourceInformation* GetResourceInformation() override { return m_ResourceInformation; };
        sdk::Result OnStart() override;
        sdk::Result OnStop() override;
        sdk::Result OnTick() override;

    private:
        sdk::ResourceInformation* m_ResourceInformation;
    };
}

#endif // !_LUA_RESOURCE_H
