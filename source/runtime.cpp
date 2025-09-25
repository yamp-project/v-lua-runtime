#include "runtime.h"
#include "resource.h"
#include "utils.h"

namespace lua
{
    bool Init()
    {
        Runtime::GetInstance()->GetLogger().Info("Runtime::Init");
        return true;
    }

    void Shutdown()
    {
        Runtime::GetInstance()->GetLogger().Info("Runtime::Shutdown");
    }
    
    void OnResourceStart(SDK_Resource* sdkResource)
    {
        Runtime* runtime = Runtime::GetInstance();
        if (Resource* resource = runtime->GetResource(sdkResource); resource)
        {
            resource->OnStart();
            return;    
        }

        Resource* resource = runtime->CreateResource(sdkResource);
        resource->OnStart();

        runtime->GetLogger().Debug("Runtime::OnResourceStart - %s", sdkResource->name);
    }

    void OnResourceStop(SDK_Resource* sdkResource)
    {
        Runtime::GetInstance()->GetLogger().Debug("Runtime::OnResourceStop - %s", sdkResource->name);
    }

    void OnTick()
    {
        for (const auto& it : Runtime::GetInstance()->GetResources())
        {
            it.second->OnTick();
        }
    }

    void OnCoreEvent(CoreEventType eventType, CAnyArray* args)
    {
        Runtime* runtime = Runtime::GetInstance();
        runtime->GetLogger().Debug("ScriptRuntime::OnEvent %d", eventType);

        for (const auto& it : runtime->GetResources())
        {
            it.second->OnCoreEvent(eventType, args);
        }
    }

    void OnResourceEvent(const char* eventName, CAnyArray* args)
    {
        Runtime* runtime = Runtime::GetInstance();
        runtime->GetLogger().Debug("ScriptRuntime::OnEvent %s", eventName);

        for (const auto& it : runtime->GetResources())
        {
            it.second->OnResourceEvent(eventName, args);
        }
    }

    std::unique_ptr<Runtime> Runtime::s_Runtime = nullptr;

    Runtime* Runtime::GetInstance()
    {
        assert(s_Runtime != nullptr);
        return s_Runtime.get();
    }

    Runtime* Runtime::Initialize(SDK_Interface* sdk)
    {
        assert(s_Runtime == nullptr);
        s_Runtime = std::make_unique<Runtime>(sdk);

        // CoreEventMetas eventMetas = s_Runtime->GetLookupTable()->GetCoreEventMetas();
        // printf("Runtime::Initialize2.1\n");
        // for (size_t i = 0; i < eventMetas.size; ++i)
        // {
        //     CoreEventMeta& eventMeta = eventMetas.buffer[i];
        //     s_Runtime->m_CoreEventMapping[::utils::StrToCamelCase(eventMeta.name)] = eventMeta.type;
        // }

        return s_Runtime.get();
    }

    void Runtime::Shutdown()
    {
        assert(s_Runtime != nullptr);
        s_Runtime.reset();
    }

    Runtime::Runtime(SDK_Interface* sdk) :
        m_Sdk(sdk),
        m_Logger(Logger(sdk, "lua")),
        m_Resources()
    {
        //
    }

    Resource* Runtime::CreateResource(SDK_Resource* sdkResource)
    {
        auto resourcePtr = std::make_unique<Resource>(m_Sdk, sdkResource);
        Resource* resource = resourcePtr.get();

        m_Resources[sdkResource] = std::move(resourcePtr);
        m_ResourceMapping[resource->GetLuaState()] = resource;

        return resource;
    }

    Resource* Runtime::GetResource(SDK_Resource* sdkResource)
    {
        auto it = m_Resources.find(sdkResource);
        if (it != m_Resources.end())
        {
            return it->second.get();
        }

        return nullptr;
    }

    Resource* Runtime::GetResource(lua_State* state)
    {
        auto it = m_ResourceMapping.find(state);
        if (it != m_ResourceMapping.end())
        {
            return it->second;
        }

        return nullptr;
    }

    std::optional<CoreEventType> Runtime::GetCoreEventType(const char* eventName)
    {
        auto it = m_CoreEventMapping.find(eventName);
        if (it != m_CoreEventMapping.end()) {
            return std::optional{it->second};
        }

        return std::nullopt;
    }
}
