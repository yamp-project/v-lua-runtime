extern "C"
{
#include <c-sdk/lookup_table.h>
#include <c-sdk/resource.h>
}

#include "resource.h"
#include "runtime.h"

#include "wrapper/lua_wrapper.h"

#include "definitions/native.h"

#include <fstream>

lua::Resource::Resource(SdkLookupTable* lookupTable, SdkResource* resource) :
    m_Runtime(lua::Runtime::GetInstance())
{
    m_LookupTable = lookupTable;
    m_Resource = resource;
    m_Logger = lua::Logger::Get("lua::Resource");
    m_State = new lua::State(m_Resource->m_Name);

    RegisterDefinition<lua::Definitions::Native>();
}

int l_my_print(lua_State *L)
{
    static auto logger = lua::Logger::Get("print");
    std::string strBuilder = "";

    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i)
    {
        strBuilder.append(std::format("{}{}", (i == 1 ? "" : " "), luaL_tolstring(L, i, nullptr)));
        lua_pop(L, 1);
    }

    logger->Info(strBuilder.c_str());
    return 0;
}

void lua::Resource::OnStart()
{
    std::filesystem::path resourcePath = m_Resource->m_Path;
    std::filesystem::path mainFilePath = resourcePath / m_Resource->m_MainFile;

    //printf("Main file path: %s\n", mainFilePath.string().c_str());
    m_Logger->Info("Main file path: %s", mainFilePath.string().c_str());

    if (!std::filesystem::exists(mainFilePath))
        return;

    m_State->GetGlobalNamespace();
    {
        m_State->RegisterCFunction("print", l_my_print);
    }
    m_State->EndNamespace();

    for(auto& definition : m_Definitions)
        definition->Initialize();

    m_Logger->Info("Running the main file: %s", mainFilePath.string().c_str());
    m_State->RunFile(mainFilePath.string(), resourcePath.string());
}

void lua::Resource::OnStop()
{

}

void lua::Resource::OnTick()
{

}