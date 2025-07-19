#include <wrapper/proxy_functions.h>

#include <assert.h>

int lua::Proxy::CApiClassIndex(lua_State* L)
{
    //for latter reference: if an object instance is called the first value on the stack is userdata
    //if the first value is a table we are possibly calling to access a static function from the metatable

    assert(lua_isuserdata(L, 1) || lua_istable(L, 1));
    lua_getmetatable(L, 1);

    if (!lua_istable(L, -1))
    {
        printf("No metatable was found in the userdata!\n");
        return 0;
    }

    //search in the metatable
    lua_pushvalue(L, 2); //push the name of the accessible thing
    lua_rawget(L, -2);

    //we found a function
    if (lua_iscfunction(L, -1))
    {
        //return the function only from the stack
        return 1;
    }

    //pop the hopefully "nil" value from the stack
    lua_pop(L, 1);

    lua_rawgetfield(L, -1, "__get");
    lua_pushvalue(L, 2);
    lua_rawget(L, -2);

    if (lua_iscfunction(L, -1))
    {
        lua_pushvalue(L, 1);
        lua_call(L, 1, 1);

        return 1;
    }

    return 0;
}