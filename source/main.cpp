#include <yamp-sdk/sdk.h>
#include "runtime.h"

IRuntimeContext GetRuntimeContext()
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
        .OnEvent         = lua::OnEvent
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
                test.MemberVariable("resourcePath", +[](IResource* ptr){ return ptr->resourcePath; });
                test.MemberVariable("resourceMainFile", +[](IResource* ptr){ return ptr->resourceMainFile; });
            }
            test.EndClass();

            IResource resource;
            resource.name = _strdup("Test resource name");
            resource.resourcePath = _strdup("Test resource path");
            resource.resourceMainFile = _strdup("Test resource main file");
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
