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
