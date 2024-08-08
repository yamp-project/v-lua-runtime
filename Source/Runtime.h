#ifndef _LUA_RUNTIME_H
#define _LUA_RUNTIME_H

#include <v-sdk/Runtime.hpp>

#include <vector>

namespace lua
{
    namespace sdk = yamp::sdk;
    class Resource;
    class Runtime : public sdk::IRuntimeBase
    {
    public:
        Runtime();
        ~Runtime();

        inline const char* GetName() override { return "Lua"; }
        const char* GetDescription() override { return "Very first Lua runtime in yamp."; }

        void OnStart() override;
        void OnStop() override;
        void OnTick() override;

        sdk::Result OnHandleResourceLoad(sdk::ResourceInformation* Information) override;

    private:
        std::vector<Resource*> m_LoadedResources;
    };
}

#endif // !_LUA_RUNTIME_H
