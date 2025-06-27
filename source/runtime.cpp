#include "runtime.h"
#include "resource.h"

namespace lua
{
    bool lua::Init()
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
        Resource* luaResource = new Resource(runtime->GetLookupTable(), resource);
        luaResource->OnStart();
    
        runtime->GetLogger()->Debug("Runtime::OnResourceStart - %s", resource->GetName());
    }
    
    void OnResourceStop(IResource* resource)
    {
        Runtime::GetInstance()->GetLogger()->Debug("Runtime::OnResourceStop");
    }

    void OnTick()
    {
        //
    }

    void OnEvent(void* event)
    {
        Runtime::GetInstance()->GetLogger()->Debug("ScriptRuntime::OnEvent");
    }
}
