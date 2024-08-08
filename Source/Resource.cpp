#include <Resource.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <print>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>

#include <v-sdk/factories/NativeFactory.hpp>
#include <v-sdk/misc/Vector.hpp>

#include <vector>

using namespace lua;
Resource::Resource(sdk::ResourceInformation* resourceInformation) :
    m_ResourceInformation(resourceInformation)
{
    
}

Resource::~Resource()
{

}

void lua_dumptable(lua_State* L, int idx, int level)
{
    std::string levelTab;
    levelTab.insert(0, level, '\t');

    lua_pushnil(L);  /* first key */
    while (lua_next(L, idx) != 0)
    {
        //we need to copy the key and value so lua_tostring will not convert it to other type
        lua_pushvalue(L, -2);

        //check if value is table
        if (lua_istable(L, -2))
        {
            //make it beautifully aligned
            std::print("{}{} => Table\n", levelTab, std::string(luaL_tolstring(L, -1, NULL)));
            std::print("{}{{\n", levelTab);

            lua_pop(L, 1);

            //for some reason we need to pass the original table and not the referenced one (recursive call)
            lua_dumptable(L, -3, level + 1);

            //make it beautifuly aligned
            std::print("{}}}\n", levelTab);
        }
        else
        {
            //note: lua_tostring will crash if userdata doesn't have any __tostring metaevent implemented
            std::string stackKey(lua_isstring(L, -1) ? lua_tostring(L, -1) : lua_typename(L, lua_type(L, -1)));
            std::string stackValue(lua_isstring(L, -2) ? lua_tostring(L, -2) : lua_typename(L, lua_type(L, -2)));
            std::print("{}{} => {},\n", levelTab, stackKey,  stackValue);
        }

        lua_pop(L, 2);
    }
}

void lua_stacktrace(lua_State* L, const char* stackName)
{
    int stackTop = lua_gettop(L);

    std::print(" --------- Stack Begins: {}---------\n", std::string(stackName));
    for (int i = stackTop; i >= 1; i--)
    {
        int valueType = lua_type(L, i);
        int relativePosition = ((i - stackTop) - 1);

        switch (valueType)
        {
        case LUA_TSTRING:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'\n", std::to_string(i), std::to_string(relativePosition), luaL_checkstring(L, i));
            break;
        case LUA_TBOOLEAN:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'\n", std::to_string(i), std::to_string(relativePosition), (lua_toboolean(L, i) ? ("true") : ("false")));
            break;
        case LUA_TNUMBER:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value: '{}'\n", std::to_string(i), std::to_string(relativePosition), std::to_string(luaL_checknumber(L, i)));
            break;
        case LUA_TTABLE:
            std::print("\tAbsolute: {} ~~ Relative: {} ~~\t Value '{}'\n", std::to_string(i), std::to_string(relativePosition), lua_typename(L, valueType));
            std::print("\t{{\n");

            lua_dumptable(L, i, 2);

            std::print("\t}}\n");
            /*lua_pushnil(L);
            while (lua_next(L, i) != 0)
            {
            Core->LogInfo("%s => %s", lua_tostring(L, -2), lua_tostring(L, -1));
            lua_pop(L, 1);
            }*/

            break;
        default:
            //std::print("\tAbsolute: {}~~ Relative: {} ~~\t Value type: {}\n", i, relativePosition, lua_typename(L, valueType));
            break;
        }
    }
    std::print(" --------- Stack Ends: {} ---------\n", stackName);
}

#include <bit>
#include <array>

#pragma pack(push, 1)
typedef struct
{
    float x;
    float __padding_x;
    float y;
    float __padding_y;
    float z;
    float __padding_z;
} NVector32;
#pragma pack(pop)

struct NVector3
{
public:
    NVector3(float x, float y, float z) :
        x(x), y(y), z(z) { }

    float x, pad0, y, pad1, z, pad2;
};

void PushArg(lua_State* L, int idx, sdk::NativeValueType type)
{
    sdk::IInvokerFactory* invokerFactory = sdk::IInvokerFactory::GetInstance();

    int luaType = lua_type(L, idx);
    switch (luaType)
    {
        case LUA_TBOOLEAN: invokerFactory->PushArgumentAsType<bool>(lua_toboolean(L, idx)); break;
        case LUA_TNUMBER:
        {
            switch (type)
            {
            case sdk::NativeValueType::Float: invokerFactory->PushArgumentAsType<float>((float)lua_tonumber(L, idx)); break;
            case sdk::NativeValueType::Int: invokerFactory->PushArgumentAsType(lua_tointeger(L, idx)); break;
            case sdk::NativeValueType::UnsignedInt: invokerFactory->PushArgumentAsType(lua_tounsigned(L, idx)); break;
            }
            break;
        }
        case LUA_TVECTOR:
        {
            const float* vector = lua_tovector(L, idx);
            invokerFactory->PushArgumentAsType<NVector32>(NVector32(vector[0], 0, vector[1], 0, vector[2], 0));
            break;
        }
        case LUA_TSTRING: invokerFactory->PushArgumentAsType<const char*>(_strdup(lua_tostring(L, idx))); break;
        default: break;
    }
}

struct ArgPtr
{
    void* m_Ptr;
    sdk::NativeValueType m_Type;
};

template<typename T>
T* CreatePtr(T value, size_t additionalSize = 1)
{
    T* ptr = static_cast<T*>(malloc(additionalSize * sizeof(T)));
    if(value != nullptr && ptr != nullptr)
        *ptr = value;

    return ptr;
}

template<typename T>
T* CreatePtr(size_t additionalSize = 1)
{
    T* ptr = static_cast<T*>(malloc(additionalSize * sizeof(T)));
    return ptr;
}

ArgPtr PushArgPtr(lua_State* L, int idx, sdk::NativeValueType type)
{
    printf("PushArgPtr1: %d\n", type);

    sdk::IInvokerFactory* invokerFactory = sdk::IInvokerFactory::GetInstance();
    void* buffer = nullptr;

    printf("PushArgPtr2\n");

    switch (type)
    {
    case sdk::NativeValueType::Bool: buffer = CreatePtr<bool>(); break;
    case sdk::NativeValueType::Float: buffer = CreatePtr<float>(); break;
    case sdk::NativeValueType::Int: buffer = CreatePtr<int>(); break;
    case sdk::NativeValueType::UnsignedInt: buffer = CreatePtr<unsigned int>(); break;
    case sdk::NativeValueType::Vector: buffer = CreatePtr<NVector32>(); break;
    case sdk::NativeValueType::String: printf("String: %s\n", lua_tostring(L, idx)); buffer = *CreatePtr<char*>(_strdup(lua_tostring(L, idx))); break;
    }

    switch (type)
    {
    case sdk::NativeValueType::Bool: invokerFactory->PushArgumentAsType<bool*>((bool*)buffer); break;
    case sdk::NativeValueType::Float: invokerFactory->PushArgumentAsType<float*>((float*)buffer); break;
    case sdk::NativeValueType::Int: invokerFactory->PushArgumentAsType<int*>((int*)buffer); break;
    case sdk::NativeValueType::UnsignedInt: invokerFactory->PushArgumentAsType<unsigned int*>((unsigned int*)buffer); break;
    case sdk::NativeValueType::Vector: invokerFactory->PushArgumentAsType<NVector32*>((NVector32*)buffer); break;
    case sdk::NativeValueType::String: invokerFactory->PushArgumentAsType<char*>((char*)buffer); break;
    }

    return ArgPtr{ buffer, type };

    //int luaType = lua_type(L, idx);
    /*switch (type)
    {
    case LUA_TBOOLEAN: invokerFactory->PushArgumentAsType<bool&>(*CreatePtr<bool>(type)); break;
    case LUA_TNUMBER:
    {
        switch (type)
        {
        case sdk::NativeValueType::Bool: invokerFactory->PushArgumentAsType<bool&>(*CreatePtr<bool>(type)); break;
        case sdk::NativeValueType::Float: invokerFactory->PushArgumentAsType<float&>(*CreatePtr<float>(type)); break;
        case sdk::NativeValueType::Int:
        {
            int tmp = lua_tointeger(L, i);
            printf("PUSH AS INTEGER!!!! %d\n", tmp);
            invokerFactory->PushArgumentAsType(tmp);
            break;
        }
        case sdk::NativeValueType::UnsignedInt:
        {
            printf("PUSH AS UINT!!!!!! %d\n", lua_tounsigned(L, i));
            invokerFactory->PushArgumentAsType(lua_tounsigned(L, i));
            break;
        }
        }
        break;
    }
    case LUA_TVECTOR:
    {
        if (isPointer)
        {
            NVector32 tmp;
            invokerFactory->PushArgumentAsType<NVector32&>(tmp);
        }
        else
        {
            const float* vector = lua_tovector(L, i);
            invokerFactory->PushArgumentAsType<NVector32>(NVector32(vector[0], 0, vector[1], 0, vector[2], 0));
        }
        break;
    }
    case LUA_TSTRING:
    {
        invokerFactory->PushArgumentAsType<const char*>(_strdup(lua_tostring(L, i)));
        break;
    }
    default:
    {
        printf("DEFAULT CASE IN ARGUMENT!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        break;
    }
    }*/
}

void PushValue(lua_State* L, sdk::NativeValueType type)
{
    printf("PushValue1\n");
    sdk::IInvokerFactory* invokerFactory = sdk::IInvokerFactory::GetInstance();

    printf("PushValue2\n");
    switch (type)
    {
    case sdk::NativeValueType::Bool: lua_pushboolean(L, invokerFactory->GetReturnValueAs<bool>()); break;
    case sdk::NativeValueType::Float: lua_pushnumber(L, invokerFactory->GetReturnValueAs<float>()); break;
    case sdk::NativeValueType::Int: lua_pushinteger(L, invokerFactory->GetReturnValueAs<int>()); break;
    case sdk::NativeValueType::UnsignedInt: lua_pushunsigned(L, invokerFactory->GetReturnValueAs<uint32_t>()); break;
    case sdk::NativeValueType::String: lua_pushstring(L, invokerFactory->GetReturnValueAs<const char*>()); break;
    case sdk::NativeValueType::Vector:
    {
        NVector32 vector = invokerFactory->GetReturnValueAs<NVector32>();
        printf("Vector3: %f | %f | %f\n", vector.x, vector.y, vector.z);
        lua_pushvector(L, vector.x, vector.y, vector.z);
        break;
    }
    }

    printf("PushValue3\n");
}

void PushValuePtr(lua_State* L, sdk::NativeValueType type, void* buffer)
{
    printf("PushValuePtr1\n");
    sdk::IInvokerFactory* invokerFactory = sdk::IInvokerFactory::GetInstance();

    printf("PushValuePtr2\n");
    switch (type)
    {
    case sdk::NativeValueType::Bool: printf("PushValuePtr2.1\n"); lua_pushboolean(L, *(bool*)buffer); break;
    case sdk::NativeValueType::Float: printf("PushValuePtr2.2\n"); lua_pushnumber(L, *(float*)buffer); break;
    case sdk::NativeValueType::Int: printf("PushValuePtr2.3\n"); lua_pushinteger(L, *(int*)buffer); break;
    case sdk::NativeValueType::UnsignedInt: printf("PushValuePtr2.4\n"); lua_pushunsigned(L, *(unsigned int*)buffer); break;
    case sdk::NativeValueType::String: printf("PushValuePtr2.5\n"); lua_pushstring(L, *(const char**)buffer); break;
    case sdk::NativeValueType::Vector:
    {
        printf("PushValuePtr2.6\n");
        NVector32 vector = *(NVector32*)buffer;
        printf("Vector3: %f | %f | %f\n", vector.x, vector.y, vector.z);
        lua_pushvector(L, vector.x, vector.y, vector.z);
        break;
    }
    }

    printf("PushValuePtr3\n");
}

int InvokeNative(lua_State* L)
{
    //uint32_t hi = lua_tounsigned(L, lua_upvalueindex(2));
    //uint32_t lo = lua_tounsigned(L, lua_upvalueindex(1));

    sdk::NativeInformation* nativeInformation = reinterpret_cast<sdk::NativeInformation*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (nativeInformation == nullptr)
        return 0;

    //uint64_t nativeHash = (uint64_t(hi) << 32) | lo;

    sdk::IInvokerFactory* invokerFactory = sdk::IInvokerFactory::GetInstance();

    lua_stacktrace(L, "BEFORE NATIVE");

    printf("---------------------------------------------------------------------\n");
    /*invokerFactory->BeginCall();
    invokerFactory->PushArgumentAsType((int)2);
    invokerFactory->PushArgumentAsType((bool)false);
    invokerFactory->EndCall(0xD1A6A821F5AC81DB);
    NVector32 h2 = invokerFactory->GetReturnValueAs<NVector32>();
    printf("InvokeShit: %f | %f | %f\n", h2.x, h2.y, h2.z);*/
    printf("---------------------------------------------------------------------\n");

    lua_stacktrace(L, "AFTER NATIVE");

    std::vector<ArgPtr> m_ReturnPointers;
    if (invokerFactory)
    {
        printf("Call native: %s | 0x%llx | %d\n", nativeInformation->m_Name, nativeInformation->m_Hash, nativeInformation->m_ReturnValue.m_Type);
        invokerFactory->BeginCall();

        int ptrArgumentCount = std::count_if(
            nativeInformation->m_ParameterValueArrayData,
            nativeInformation->m_ParameterValueArrayData + nativeInformation->m_ParameterValueArraySize,
            [](const sdk::NativeValueInformation& information)
            {
                return information.m_IsPointer;
            });

        printf("ptrArgumentCOunt: %d\n", ptrArgumentCount);
        /*if (ptrArgumentCount)
            return 0;*/

        int skippingArgPtr = 0;
        for (int i = 1; i <= lua_gettop(L); i++)
        {
            sdk::NativeValueInformation nativeParameter = nativeInformation->m_ParameterValueArrayData[i - 1];
            bool isPointer = nativeParameter.m_IsPointer;

            if (!isPointer)
                PushArg(L, i, nativeParameter.m_Type);
            else
            {
                PushArgPtr(L, i, nativeParameter.m_Type);
                skippingArgPtr++;
            }

            printf("Argument!!! %d\n", lua_type(L, i));
        }

        for (int x = skippingArgPtr; x < nativeInformation->m_ParameterValueArraySize; x++)
        {
            auto nativeParameter = nativeInformation->m_ParameterValueArrayData[x];
            if(nativeInformation->m_ParameterValueArrayData[x].m_IsPointer)
                m_ReturnPointers.push_back(PushArgPtr(L, x, nativeParameter.m_Type));
        }

        printf("END CALL: 0x%llx\n", nativeInformation->m_Hash);
        invokerFactory->EndCall(nativeInformation->m_Hash);
        printf("AFTER END CALL!!!\n");

        printf("Native was successfully invoked! %p\n", nativeInformation);
        //if (nativeInformation->m_ReturnValue.m_Type != sdk::NativeValueType::Void)
        {
            PushValue(L, nativeInformation->m_ReturnValue.m_Type);
            for (auto& ret : m_ReturnPointers)
            {
                PushValuePtr(L, ret.m_Type, ret.m_Ptr);
                delete ret.m_Ptr;
            }

            int defaultReturn = nativeInformation->m_ReturnValue.m_Type != sdk::NativeValueType::Void;
            return defaultReturn + m_ReturnPointers.size();
        }
    }
    printf("InvokeNative!!!!! %s\n", nativeInformation->m_Name);

    return 0;
}

void RegisterNatives(lua_State* L)
{
    sdk::INativeReflectionFactory* nativeReflectionFactory = sdk::INativeReflectionFactory::GetInstance();
    
    lua_newtable(L);
    //lua_pushstring(L, "native");
    //lua_newtable(L);
    //lua_settable(L, -3);
    

    for (auto& native : nativeReflectionFactory->GetListOfNatives())
    {
        sdk::NativeInformation* nativeInformation = nativeReflectionFactory->GetNativeInformation(native);
        if (nativeInformation == nullptr)
            continue;

        //printf("Native information: %s - 0x%llx\n", nativeInformation->m_Name, nativeInformation->m_Hash);

        lua_pushstring(L, nativeInformation->m_Name);
        lua_pushlightuserdata(L, nativeInformation);
        //lua_pushunsigned(L, uint32_t(native));
        //lua_pushunsigned(L, uint32_t(native >> 32));
        lua_pushcclosure(L, InvokeNative, "InvokeNative", 1);
        lua_settable(L, -3);
        //printf("Native: 0x%llx | %s\n", native);
    }

    //lua_stacktrace(L, "RegisterNatives");

    lua_setglobal(L, "native");
}

sdk::Result Resource::OnStart()
{
    printf("Resource::OnStart\n");
    
    lua_State* resourceState = luaL_newstate();
    luaL_openlibs(resourceState);

    printf("Reg\n");
    RegisterNatives(resourceState);

    sdk::IInvokerFactory* invokerFactory = sdk::IInvokerFactory::GetInstance();
    invokerFactory->BeginCall();
    invokerFactory->PushArgumentAsType((int)2);
    invokerFactory->PushArgumentAsType((bool)false);
    invokerFactory->EndCall(0xD1A6A821F5AC81DB);
    NVector32 h = invokerFactory->GetReturnValueAs<NVector32>();
    printf("HHHHHHHHHHH: %f | %f | %f\n", h.x, h.y, h.z);

    invokerFactory->BeginCall();
    invokerFactory->PushArgumentAsType((int)2);
    invokerFactory->PushArgumentAsType((bool)false);
    invokerFactory->EndCall(0xD1A6A821F5AC81DB);
    NVector32 h2 = invokerFactory->GetReturnValueAs<NVector32>();
    printf("HHHHHHHHHHH: %f | %f | %f\n", h2.x, h2.y, h2.z);

    invokerFactory->BeginCall();
    invokerFactory->EndCall(0x4A8C381C258A124D);
    int playerPed = invokerFactory->GetReturnValueAs<int>();
    printf("PED: %d\n", playerPed);

    std::filesystem::path resourcePath = m_ResourceInformation->m_Path;
    std::filesystem::path mainFilePath = resourcePath / m_ResourceInformation->m_MainFile;

    if (std::filesystem::exists(mainFilePath))
    {
        

        printf("Main file exists! %s\n", mainFilePath.string().c_str());
        std::ifstream mainFileStream(mainFilePath);

        std::string content((std::istreambuf_iterator<char>(mainFileStream)), (std::istreambuf_iterator<char>()));
        printf("Content: %lld\n", content.size());

        size_t bytecodeSize = 0;
        char* bytecode = luau_compile(content.c_str(), content.size(), NULL, &bytecodeSize);
        int result = luau_load(resourceState, "test", bytecode, bytecodeSize, 0);
        free(bytecode);

        lua_pcall(resourceState, 0, 0, 0);

        printf("Result: %d\n", result);

        if (result == 0)
            return { true };
    }

    printf("Main file: %s\n", mainFilePath.string().c_str());

    lua_close(resourceState);

    return { true };
}

sdk::Result Resource::OnStop()
{
    printf("Resource::OnStop\n");
    return { true };
}

sdk::Result Resource::OnTick()
{
    //printf("Resource::OnTick\n");
    return { true };
}