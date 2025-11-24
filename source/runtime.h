#pragma once

#include <yamp-sdk/sdk.h>
#include <unordered_map>
#include <assert.h>

#include "logger.h"
#include "resource.h"

namespace lua
{
    bool Init();
    void Shutdown();

    void OnResourceStart(SDK_Resource* sdkResource);
    void OnResourceStop(SDK_Resource* sdkResource);
    void OnTick();
    void OnCoreEvent(CoreEventType eventType, CAnyArray* args);
    void OnResourceEvent(const char* eventName, CAnyArray* args);
    SDK_ResourceStatistic* OnResourceStatistic(SDK_Resource * resource);

    class Resource;
    class Runtime
    {
    public:
        using Resources = std::unordered_map<SDK_Resource*, std::unique_ptr<Resource>>;

        static Runtime* GetInstance();
        static Runtime* Initialize(SDK_Interface* sdk);
        static void Shutdown();

        Runtime(SDK_Interface* sdk);
        ~Runtime() = default;

        SDK_Interface* GetSdkInterface() { return m_Sdk; }
        Resources& GetResources() { return m_Resources; }
        Logger& GetLogger() { return m_Logger; }
        
        Resource* CreateResource(SDK_Resource* sdkResource);
        Resource* GetResource(SDK_Resource* sdkResource);
        Resource* GetResource(lua_State* state);

        std::optional<CoreEventType> GetCoreEventType(const char* eventName);

    private:
        static std::unique_ptr<Runtime> s_Runtime;

        SDK_Interface* m_Sdk;
        Resources m_Resources;
        Logger m_Logger;

        std::unordered_map<std::string, CoreEventType> m_CoreEventMapping;
        std::unordered_map<lua_State*, Resource*> m_ResourceMapping;
    };
}
