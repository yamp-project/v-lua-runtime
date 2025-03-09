#pragma once

struct lua_State;

namespace lua::Utils
{
    void lua_dumptable(lua_State* L, int idx, int level);
    void lua_stacktrace(lua_State* L, const char* stackName);

    int traceback(lua_State* L);
}