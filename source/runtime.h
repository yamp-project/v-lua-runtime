#pragma once

#include <unordered_map>
#include <assert.h>

#include "logger.h"
#include "resource.h"

namespace lua
{
    bool Init();
    void Shutdown();

    void OnResourceStart(IResource* iResource);
    void OnResourceStop(IResource* iResource);
    void OnTick();
    void OnEvent(CoreEvent event);

    class Resource;
    class Runtime
    {
    public:
        static Runtime* GetInstance();
        static Runtime* Initialize(ILookupTable* lookupTable);
        static void Shutdown();

        Runtime(ILookupTable* lookupTable);
        ~Runtime() = default;

        Resource* CreateResource(IResource* iResource);

        ILookupTable* GetLookupTable() {
            return m_LookupTable;
        }

        Logger* GetLogger()
        {
            return &m_Logger;
        }

        Resource* GetStateResource(lua_State* state)
        {
            return m_States[state];
        }

    // private:
        static std::unique_ptr<Runtime> s_Runtime;

        ILookupTable* m_LookupTable;
        Logger m_Logger;

        std::unordered_map<IResource*, std::unique_ptr<Resource>> m_Resources;
        std::unordered_map<lua_State*, Resource*> m_States;
    };
}
