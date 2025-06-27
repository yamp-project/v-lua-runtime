#include "resource.h"
#include "runtime.h"

#include "wrapper/lua_wrapper.h"

#include "definitions/native.h"

#include <filesystem>
#include <fstream>

namespace lua
{
    Resource::Resource(ILookupTable* lookupTable, IResource* resource)
    {
        m_Logger = std::make_unique<Logger>(lookupTable, std::format("resource {}", resource->GetName()));
        // m_State = new lua::State(m_Resource->GetName());
        m_State = std::make_unique<lua::State>("test");
        m_Resource = resource;
    
        RegisterDefinition<lua::Definitions::Native>();
    }

    int l_my_print(lua_State *L)
    {
        // static auto logger = lua::Logger::Get("print");
        std::string strBuilder = "";
    
        int nargs = lua_gettop(L);
        for (int i = 1; i <= nargs; ++i)
        {
            strBuilder.append(std::format("{}{}", (i == 1 ? "" : " "), luaL_tolstring(L, i, nullptr)));
            lua_pop(L, 1);
        }
    
        // logger->Info(strBuilder.c_str());
        printf("%s\n", strBuilder.c_str());
        return 0;
    }
    
    void Resource::OnStart()
    {
        // TODO: temporary
        std::filesystem::path resourcePath = "C:\\Documents\\gta\\project\\v-lua-runtime\\resource";
        std::filesystem::path mainFilePath = resourcePath / "main.lua";
    
        //printf("Main file path: %s\n", mainFilePath.string().c_str());
        // m_Logger->Info("Main file path: %s", mainFilePath.string().c_str());
        printf("Main file path: %s", mainFilePath.string().c_str());
    
        if (!std::filesystem::exists(mainFilePath))
        {
            return;
        }
    
        m_State->GetGlobalNamespace();
        {
            m_State->RegisterCFunction("print", l_my_print);
        }
        m_State->EndNamespace();
    
        for(auto& definition : m_Definitions)
            definition->Initialize();

        // m_Logger->Info("Running the main file: %s", mainFilePath.string().c_str());
        m_State->RunFile(mainFilePath.string(), resourcePath.string());
    }
    
    void Resource::OnStop()
    {
        //
    }

    void Resource::OnTick()
    {
        //
    }
} // namespace lua