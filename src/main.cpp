#include <Windows.h>
#include <iostream>

#include <v-sdk/factories/RuntimeFactory.hpp>
#include <Runtime.h>

namespace sdk = yamp::sdk;
BOOL WINAPI DllMain(HINSTANCE instanceDll, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        auto runtimeFactory = yamp::sdk::IRuntimeFactory::GetInstance();
        
        printf("[Lua] Runtime has been loaded2! %p\n", reserved);
        printf("[Lua] Author: yamp\n");
        printf("[Lua] Version: 0.0.1-alpha\n");
        printf("[Lua] Runtime factory instance: %p\n\n", runtimeFactory);

        lua::Runtime* runtime = new lua::Runtime;
        sdk::Result result = runtimeFactory->RegisterRuntime(runtime);
        result = runtimeFactory->RegisterRuntimeResourceHandling("lua", runtime);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        printf("[Lua] Runtime is unloading...\n");
    }

    return true;
}