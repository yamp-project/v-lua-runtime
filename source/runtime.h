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
    void OnCoreEvent(CoreEventType eventType, CAnyArray* args);
    void OnResourceEvent(const char* eventName, CAnyArray* args);

    class Resource;
    class Runtime
    {
    public:
        static Runtime* GetInstance();
        static Runtime* Initialize(ILookupTable* lookupTable);
        static void Shutdown();

        using Resources = std::unordered_map<IResource*, std::unique_ptr<Resource>>;

        Runtime(ILookupTable* lookupTable);
        ~Runtime() = default;

        ILookupTable* GetLookupTable() { return m_LookupTable; }
        Resources& GetResources() { return m_Resources; }
        Logger& GetLogger() { return m_Logger; }
        
        Resource* CreateResource(IResource* iResource);
        Resource* GetResource(IResource* iResource);
        Resource* GetResource(lua_State* state);

        std::optional<CoreEventType> GetCoreEventType(const char* eventName);

    private:
        static std::unique_ptr<Runtime> s_Runtime;

        ILookupTable* m_LookupTable;
        Resources m_Resources;
        Logger m_Logger;

        std::unordered_map<std::string, CoreEventType> m_CoreEventMapping;
        std::unordered_map<lua_State*, Resource*> m_ResourceMapping;
    };
}
