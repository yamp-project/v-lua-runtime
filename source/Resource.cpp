#include <v-sdk/lookup_table.h>

#include "Resource.h"
#include "Runtime.h"

#include <v-sdk/resource.h>

lua::Resource::Resource(SdkResource* resource) :
    m_Runtime(lua::Runtime::GetInstance())
{
    m_Resource = resource;
}

void lua::Resource::OnStart()
{

}

void lua::Resource::OnStop()
{

}

void lua::Resource::OnTick()
{

}