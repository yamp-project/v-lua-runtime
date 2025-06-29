#include "resource.h"
#include "runtime.h"

#include "wrapper/lua_wrapper.h"
#include "definitions/native.h"

#include <filesystem>
#include <fstream>

namespace lua
{
    int l_my_print(lua_State *L)
    {
        Logger* logger = Runtime::GetInstance()->GetLogger();
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

    Resource::Resource(ILookupTable* lookupTable, IResource* resource) :
        m_Logger(Logger(lookupTable, std::format("resource {}", m_Resource->name))),
        m_State(State(m_Resource->name)),
        m_Resource(resource)
    {
        RegisterDefinition<lua::Definitions::Native>();
    }

    void Resource::OnStart()
    {
        std::filesystem::path resourcePath = m_Resource->resourcePath;
        std::filesystem::path mainFilePath = resourcePath / m_Resource->resourceMainFile;
    
        m_Logger.Info("Main file path: %s", mainFilePath.string().c_str());

        if (!std::filesystem::exists(mainFilePath))
        {
            return;
        }

        m_State.GetGlobalNamespace();
        m_State.RegisterCFunction("print", l_my_print);
            m_State.BeginNamespace("yamp");
                m_State.RegisterVariable("side", "client");
            m_State.EndNamespace();
        m_State.EndNamespace();

        for(auto& definition : m_Definitions)
        {
            definition->Initialize();
        }

        m_Logger.Info("Running the main file: %s", mainFilePath.string().c_str());
        m_State.RunFile(mainFilePath.string(), resourcePath.string());
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