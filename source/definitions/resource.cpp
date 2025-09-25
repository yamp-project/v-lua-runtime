#include "wrapper/lua_wrapper.h"
#include "definition.h"
#include "resource.h"
#include "runtime.h"

#include <iomanip>

int32_t RegisterCoreEvent(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Expected an event name");
        return 1;
    }

    if (!lua_isfunction(L, 2)) {
        luaL_error(L, "Expected a function");
        return 1;
    }

    lua::Runtime* runtime = lua::Runtime::GetInstance();

    std::optional<CoreEventType> eventType = runtime->GetCoreEventType(lua_tostring(L, 1));
    if (!eventType)
    {
        luaL_error(L, "Unknown event type");
        return 1;
    }

    lua::Resource* resource = runtime->GetResource(L);
    resource->RegisterCoreCallbackRef(*eventType, lua::utils::luaL_ref(L, LUA_REGISTRYINDEX));
    return 0;
}

int32_t RegisterResourceEvent(lua_State* L)
{
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Expected an event name");
        return 1;
    }

    if (!lua_isfunction(L, 2)) {
        luaL_error(L, "Expected a function");
        return 1;
    }

    lua::Runtime* runtime = lua::Runtime::GetInstance();
    lua::Resource* resource = runtime->GetResource(L);
    resource->RegisterCallbackRef(lua_tostring(L, 1), lua::utils::luaL_ref(L, LUA_REGISTRYINDEX));

    return 0;
}

static int32_t C_Emit(lua_State *L) {
    lua::Runtime* runtime = lua::Runtime::GetInstance();
    SDK_Interface* sdk = runtime->GetSdkInterface();
    lua::Resource* resource = runtime->GetResource(L);
    const char* eventName = lua_tostring(L, 1);

    int n = lua_gettop(L); // number of arguments
    CAnyArray* array = sdk->anyValueFactory->MakeAnyArrayValue(n);

    for (int i = 2; i <= n; i++) {
        array->buffer[i] = lua::utils::MakeAnyValue(sdk->anyValueFactory, L, i);
    }

    // sdk->nativesFactory->EmitResourceEvent(eventName, array);
    return 0;
}

int l_my_print(lua_State* L)
{
    std::string strBuilder = "";

    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i)
    {
        strBuilder.append(i == 1 ? "" : " ").append(luaL_tolstring(L, i, nullptr));
        lua_pop(L, 1);
    }

    lua::Runtime::GetInstance()->GetLogger().Info(strBuilder.c_str());
    return 0;
}

static lua::StaticDefinition resourceClass([](lua::Resource* resource)
{
    lua::Runtime *runtime = lua::Runtime::GetInstance();
    SDK_Interface* sdk = runtime->GetSdkInterface();

    auto state = resource->GetState();
    state->GetGlobalNamespace();
    {
        state->RegisterCFunction("print", l_my_print);

        state->BeginNamespace("yamp");
        {
            state->BeginClass("Ped");
            {
                state->Getter("health", sdk->entityApi->GetHealth);
                state->Setter("health", sdk->entityApi->SetHealth);

                state->MemberFunction("equipWeapon", sdk->pedApi->EquipWeapon);
            }
            state->EndClass();

            state->PushObject(sdk->GetLocalPlayer(), "Ped");
            state->RegisterVariable("localPlayer");

            state->BeginClass("resource");
            {
                // state->MemberVariable("name", +[](lua::Resource* resource){ return resource->m_Resource->name; });
                // state->MemberVariable("resourcePath", +[](lua::Resource* resource){ return resource->m_Resource->path; });
                // state->MemberVariable("resourceMainFile", +[](lua::Resource* resource){ return resource->m_Resource->mainFile; });
                // state->MemberFunction("on", +[](lua::Resource* resource, const char* eventName) { printf("%p - %s\n", resource, eventName); });
            }
            state->EndClass();

            state->PushObject(resource, "resource");
            state->RegisterVariable("resource");
        }

        // state->RegisterCFunction("on", RegisterCoreEvent);
        // state->RegisterCFunction("onResource", RegisterResourceEvent);
        // state->RegisterCFunction("emit", C_Emit);

        state->EndNamespace();
    }
    state->EndNamespace();
});
