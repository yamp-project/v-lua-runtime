#include "runtime.h"
#include "resource.h"

#include <yamp-sdk/cpp/any_value.h>

namespace lua
{
    bool Init()
    {
        Runtime::GetInstance()->GetLogger()->Info("Runtime::Init");
        return true;
    }

    void Shutdown()
    {
        Runtime::GetInstance()->GetLogger()->Info("Runtime::Shutdown");
    }
    
    void OnResourceStart(IResource* iResource)
    {
        Runtime* runtime = Runtime::GetInstance();
        if (runtime->m_Resources.contains(iResource))
        {
            runtime->m_Resources[iResource]->OnStart();
            return;    
        }

        Resource* resource = runtime->CreateResource(iResource);
        resource->OnStart();

        runtime->GetLogger()->Debug("Runtime::OnResourceStart - %s", iResource->name);
    }
    
    void OnResourceStop(IResource* iResource)
    {
        Runtime::GetInstance()->GetLogger()->Debug("Runtime::OnResourceStop");
    }

    void OnTick()
    {
        //
    }

    void OnEvent(CoreEvent event)
    {
        Runtime::GetInstance()->GetLogger()->Debug("ScriptRuntime::OnEvent %d", event.type);

        for (const auto& it : Runtime::GetInstance()->m_Resources)
        {
            it.second->OnEvent(event);
        }
    }

    std::unique_ptr<Runtime> Runtime::s_Runtime = nullptr;

    Runtime* Runtime::GetInstance()
    {
        assert(s_Runtime != nullptr);
        return s_Runtime.get();
    }

    Runtime* Runtime::Initialize(ILookupTable* lookupTable)
    {
        assert(s_Runtime == nullptr);
        s_Runtime = std::make_unique<Runtime>(lookupTable);
        return s_Runtime.get();
    }

    void Runtime::Shutdown()
    {
        assert(s_Runtime != nullptr);
        s_Runtime.reset();
    }

    Runtime::Runtime(ILookupTable* lookupTable) :
        m_LookupTable(lookupTable),
        m_Logger(Logger(lookupTable, "lua")),
        m_Resources()
    {
        //
    }

    Resource* Runtime::CreateResource(IResource* iResource)
    {
        if (m_Resources.contains(iResource))
        {
            m_Logger.Error("The resource %s already exists", iResource->name);
            return m_Resources[iResource].get();
        }

        m_Resources[iResource] = std::make_unique<Resource>(m_LookupTable, iResource);
        Resource* resource = m_Resources[iResource].get();
        m_States[resource->GetState()] = resource;

        return resource;
    }
}
