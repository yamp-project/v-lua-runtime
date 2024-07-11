#include <v-sdk/factories/ResourceFactory.hpp>

#include <Runtime.h>
#include <Resource.h>

#include <iostream>

using namespace lua;
Runtime::Runtime()
{

}

Runtime::~Runtime()
{

}

void Runtime::OnStart()
{

}

void Runtime::OnStop()
{

}

void Runtime::OnTick()
{

}

sdk::Result Runtime::OnHandleResourceLoad(sdk::ResourceInformation* information)
{
    sdk::IResourceFactory* resourceFactory = sdk::IResourceFactory::GetInstance();
    lua::Resource* newResource = new lua::Resource(information);

    sdk::Result resourceCreationResult = resourceFactory->RegisterResource(newResource);

    m_LoadedResources.push_back(newResource);

    return { true };
}