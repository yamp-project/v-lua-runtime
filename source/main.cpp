#include <yamp-sdk/sdk.h>
#include "runtime.h"

SDK_Context GetRuntimeContext()
{
    return {
        .version    = "0.0.1",
        .sdkVersion = "0.0.1",

        // lifecycle
        .Init     = lua::Init,
        .Shutdown = lua::Shutdown,

        // events
        .OnResourceStart = lua::OnResourceStart,
        .OnResourceStop  = lua::OnResourceStop,
        .OnTick          = lua::OnTick,
        .OnCoreEvent     = lua::OnCoreEvent,
        .OnResourceEvent = lua::OnResourceEvent,
        .OnResourceStatistic = lua::OnResourceStatistic
    };
}

SDK_EXPORT void RuntimeEntry(RegisterRuntime registerRuntime)
{
    lua::Runtime* runtime = lua::Runtime::Initialize(registerRuntime("lua", GetRuntimeContext()));
    runtime->GetLogger().Info("runtime registered with success!");
}

#ifdef LUA_BUILD_EXE

struct TestStruct
{

};

int main(int argc, char** argv)
{

    printf("main1\n");
    lua::State test;
    printf("main2\n");

    test.GetGlobalNamespace();
    {
        test.BeginNamespace("yamp");
        {
            test.BeginClass("resource");
            {
                test.MemberVariable("name", +[](IResource* ptr){ return ptr->name; });
                test.MemberVariable("resourcePath", +[](IResource* ptr){ return ptr->path; });
                test.MemberVariable("resourceMainFile", +[](IResource* ptr){ return ptr->mainFile; });

                // test.MemberFunction("testCb", +[](IResource* resource, lua_State* L)
                // {
                //     printf("This should print the state: %p\n", L);
                //     lua::utils::lua_stacktrace(L, "testCb");
                //     return 0;
                // });

                test.MemberFunction("testCb", +[](IResource* resource, const char* eventName, lua::FunctionRef shitTest)
                {
                    printf("Testing shit! %s, %d, %p\n", eventName, shitTest.m_Reference, shitTest.m_Pointer);
                    //return 0;
                });

                test.RegisterCFunction("getByName", +[](lua_State *L)
                {
                    printf("getByName\n");
                    return 0;
                });
            }
            test.EndClass();

            IResource resource;
            resource.name = _strdup("Test resource name");
            resource.path = _strdup("Test resource path");
            resource.mainFile = _strdup("Test resource main file");
            printf("Test shit: %p\n", &resource);

            test.PushObject(&resource, "resource");
            test.RegisterVariable("thisResource");
        }

        test.EndNamespace();
    }

    test.EndNamespace();
    test.RunFile("test.lua");

    return 0;
}

#endif
