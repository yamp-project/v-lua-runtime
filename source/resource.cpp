#include "resource.h"
#include "runtime.h"

#include "wrapper/utils.h"
#include "wrapper/lua_wrapper.h"
#include "definitions/native.h"

#include <filesystem>
#include <iostream>
#include <fstream>

LUALIB_API int luaL_ref (lua_State *L, int t) {
  int ref;
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);  /* remove from stack */
    return LUA_REFNIL;  /* 'nil' has a unique fixed reference */
  }
  t = lua_absindex(L, t);
  if (lua_rawgeti(L, t, 1) == LUA_TNUMBER)  /* already initialized? */
    ref = (int)lua_tointeger(L, -1);  /* ref = t[1] */
  else {  /* first access */
    lua_toboolean(L, -1);  /* must be nil or false */
    ref = 0;  /* list is empty */
    lua_pushinteger(L, 0);  /* initialize as an empty list */
    lua_rawseti(L, t, 1);  /* ref = t[1] = 0 */
  }
  lua_pop(L, 1);  /* remove element from stack */
  if (ref != 0) {  /* any free element? */
    lua_rawgeti(L, t, ref);  /* remove it from list */
    lua_rawseti(L, t, 1);  /* (t[1] = t[ref]) */
  }
  else  /* no free elements */
    ref = (int)lua_rawlen(L, t) + 1;  /* get a new reference */
  lua_rawseti(L, t, ref);
  return ref;
}

namespace lua
{
    int lua_callback_ref = LUA_REFNIL;
    int register_callback(lua_State* L) {
        if (!lua_isfunction(L, 1)) {
            luaL_error(L, "Expected a function");
            return 1;
        }

        printf("1\n");
        lua_pushvalue(L, -1);
        lua_callback_ref = lua_ref(L, -1);
        lua::Utils::lua_stacktrace(L, "test");
        printf("2\n");
        return 0;
    }

    void call_lua_callback(lua_State* L) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_callback_ref);

        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            std::cerr << "Error calling Lua callback: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
        }
    }

    int l_my_print(lua_State* L)
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
        m_Logger(Logger(lookupTable, std::format("resource {}", resource->name))),
        m_State(State(resource->name)),
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
                m_State.RegisterCFunction("cb", register_callback);
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

    void Resource::OnEvent()
    {
        printf("from resource %p\n", m_State.GetState());
        call_lua_callback(m_State.GetState());
    }
} // namespace lua
