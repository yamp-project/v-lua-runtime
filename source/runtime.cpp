#include "runtime.h"
#include "resource.h"

extern "C"
{
#include <c-sdk/resource.h>
#include <c-sdk/lookup_table.h>
}

lua::Runtime::Runtime()
{

}

void lua::Runtime::OnStart()
{
    m_Logger->Info("Runtime::OnStart");
}

void lua::Runtime::OnStop()
{

}

void lua::Runtime::OnTick()
{

}

bool lua::Runtime::OnResourceStart(SdkResource* resource)
{
    lua::Resource* luaResource = new lua::Resource(m_LookupTable, resource);
    luaResource->OnStart();

    return true;
}

bool lua::Runtime::OnResourceStop(SdkResource* resource)
{
    return true;
}

void lua::Runtime::OnResourceTick(SdkResource* resource)
{

}
