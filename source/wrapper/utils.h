#pragma once

struct lua_State;

#include <yamp-sdk/sdk.h>
#include <assert.h>
#include <cstdint>

namespace lua::utils
{
    int luaL_ref(lua_State* L, int t);
    void lua_dumptable(lua_State* L, int idx, int level);
    void lua_stacktrace(lua_State* L, const char* stackName);

    int traceback(lua_State* L);
    void PushAnyValueToStack(lua_State* L, CAnyValue* value);
    CAnyValue* MakeAnyValue(SDK_AnyValueFactory* valueFactory, lua_State *L, int32_t index);
}
