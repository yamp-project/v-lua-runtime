#pragma once

struct lua_State;
struct CAnyValue;

namespace lua::utils
{
    void lua_dumptable(lua_State* L, int idx, int level);
    void lua_stacktrace(lua_State* L, const char* stackName);

    int traceback(lua_State* L);
    void PushAnyValueToStack(lua_State* L, CAnyValue* value);
}
