#include <iostream>
#include <vector>

#include <c-sdk/sdk.h>

#include <runtime.h>
#include <wrapper/lua_wrapper.h>

#pragma comment(lib, "D:\\Projects\\C\\yamp-project\\yamp\\cmake-build-debug\\client\\yamp_client.lib")

template<typename T>
T* CreatePtr(T value, size_t additionalSize = 1)
{
    T* ptr = static_cast<T*>(malloc(additionalSize * sizeof(T)));
    if(value != nullptr && ptr != nullptr)
        *ptr = value;

    return ptr;
}

#ifdef LUA_BUILD_EXE

struct Vector3
{
    float x = 10;
    float y = 20;
    float z = 30;
};

class SomeClass
{
public:
    SomeClass() { printf("SomeClass\n"); }
    SomeClass(int a) { printf("SomeClass2\n"); }
    ~SomeClass() { printf("~SomeClass\n"); }

    void PrintOne() { printf("One motherfucking one\n"); }
    void PrintTwo() { printf("Two\n"); }
    void DoSomething(int a, int b) { printf("Sum: %d\n", a + b); }

    Vector3 GetPosition() { return m_Position; }
    void SetPosition(Vector3 position) { m_Position = position; }

private:
    Vector3 m_Position;
};

SomeClass* SomeClass_New() { return new SomeClass; }
SomeClass* SomeClass_New2(int a) { return new SomeClass(a); }
void SomeClass_PrintOne(SomeClass* ptr) { ptr->PrintOne(); }
void SomeClass_PrintTwo(SomeClass* ptr) { ptr->PrintTwo(); }
void SomeClass_DoSomething(SomeClass* ptr, int a, int b) { ptr->DoSomething(a, b); }
Vector3 SomeClass_GetPosition(SomeClass* ptr) { return ptr->GetPosition(); }
void SomeClass_SetPosition(SomeClass* ptr, Vector3 position) { ptr->SetPosition(position); }

int main(int argNum, char** args)
{
    lua::State test;

    /*test.RegisterFunction("hihiHaha", +[](std::optional<const char*> a, std::optional<const char*> b)
    {
        if (a)
            printf("Printing others: %s\n", a.value());

        printf("Printing hihihaha: %d\n", a.has_value());
    });*/

    printf("Ha1\n");
    /*test.GetGlobalNamespace()
        .BeginNamespace("yamp")
            .RegisterVariable("test", 10)
        .EndNamespace();*/
    
    test.GetGlobalNamespace()
        .BeginNamespace("yamp")
            .BeginClass<SomeClass>("SomeClass")
                .Constructor(SomeClass_New)
                .MemberFunction("PrintOne", SomeClass_PrintOne)
                .MemberVariable("position", SomeClass_GetPosition)
            .EndClass()
        .EndNamespace();

    /*test.GetGlobalNamespace()
        .BeginNamespace("yamp")
            .RegisterVariable("test", 10)
        .EndNamespace()
        .BeginNamespace("yamp2")
            .RegisterVariable("test2", 141.124f)
        .EndNamespace()
        .BeginNamespace("yamp3")
            .BeginNamespace("yamp4")
                .BeginNamespace("yamp5")
                    .RegisterVariable("test3", true)
                .EndNamespace()
            .EndNamespace()
        .EndNamespace()
        .BeginNamespace("yamp")
            .RegisterVariable("test4", "hihi")
        .EndNamespace();

    test.GetGlobalNamespace()
        .BeginNamespace("yamp")
            .BeginClass<SomeClass>("SomeClass")
                .Constructor(SomeClass_New2)
                .MemberFunction("PrintOne", SomeClass_PrintOne)
                .MemberFunction("PrintTwo", SomeClass_PrintTwo)
            .EndClass()
        .EndNamespace();*/

    lua::Utils::lua_stacktrace(test.GetState(), "Shit");
    printf("Ha2\n");

    /*test.GetNamespace("yamp")
        .BeginClass<SomeClass>("SomeClass")
            .Constructor(SomeClass_New)
            .MemberFunction("PrintOne", SomeClass_PrintOne)
            .MemberFunction("PrintTwo", SomeClass_PrintTwo)
            .MemberFunction("DoSomething", SomeClass_DoSomething)
        .EndClass();*/

    test.RunFile("test.lua");
    printf("HAHAHAHA\n");
}

#else

SDK_EXPORT void ModuleEntry()
{
    static lua::Runtime* runtime = lua::Runtime::GetInstance();
    static auto runtimeFunctions = SdkRuntimeEntry {
        .GetName = []() { return "Lua Module"; },
        .GetDescription = []() { return "A small runtime example to load in yamp 222"; },

        .OnRuntimeStart = []() { runtime->OnStart(); },
        .OnRuntimeStop = []() { runtime->OnStop(); },
        .OnRuntimeTick = []() { runtime->OnTick(); },

        .OnResourceStart = [](SdkResource* resource) { return runtime->OnResourceStart(resource); },
        .OnResourceStop = [](SdkResource* resource) { return runtime->OnResourceStop(resource); },
        .OnResourceTick = [](SdkResource* resource) { runtime->OnResourceTick(resource); },
    };

    auto lookupTable = RegisterRuntime("lua", &runtimeFunctions);
    runtime->SetLookupTable(lookupTable);

//    static auto nativeReflection = asd->NativeReflection__Create();
//
//    int nativeSize = 0;
//    uint64_t* nativeArray = nullptr;
//    asd->NativeReflection__GetListOfNatives(nativeReflection, &nativeArray, &nativeSize);
//
//    printf("Native list size: %d\n", nativeSize);
//    for(int i = 0; i < nativeSize; i++)
//        printf("Native: 0x%llx\n", nativeArray[i]);
//
//    static auto invoker = asd->NativeInvoker__Create();
//    asd->NativeInvoker__Begin(invoker, 0x4A8C381C258A124D);
//    asd->NativeInvoker__Call(invoker);
//    auto pedId = (uint32_t*)asd->NativeInvoker__GetReturnValueBuffer(invoker);
//    printf("Hmmm: %p - %d\n", invoker, *pedId);
}

#endif