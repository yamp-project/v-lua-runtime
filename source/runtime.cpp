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
    
    void OnResourceStart(IResource* resource)
    {
        Runtime* runtime = Runtime::GetInstance();
        runtime->m_Resources[resource] = std::make_unique<Resource>(runtime->GetLookupTable(), resource);
        runtime->m_Resources[resource]->OnStart();

        runtime->GetLogger()->Debug("Runtime::OnResourceStart - %s", resource->name);
    }
    
    void OnResourceStop(IResource* resource)
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

        for (size_t i = 0; i < event.args->size; ++i)
        {
            CAnyValue* value = event.args->buffer[i];
            switch ((CType)value->type)
            {
            case CType::C_INT_32:
                printf("arg[%zu]: %d\n", i, ValueAs<int32_t>(value));
                break;

            default:
                break;
            }
        }

        for (const auto& it : Runtime::GetInstance()->m_Resources)
        {
            printf("calling ???\n");
            it.second->OnEvent();
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

    void Runtime::CreateResource(IResource* resource)
    {
        if (m_Resources.contains(resource))
        {
            m_Logger.Error("The resource %s already exists", resource->name);
            return;
        }

        m_Resources[resource] = std::make_unique<Resource>(m_LookupTable, resource);
    }
}
