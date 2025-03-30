#include "native.h"
#include "runtime.h"
#include "wrapper/lua_wrapper.h"
#include <c-sdk/lookup_table.h>

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

struct ArgPtr
{
    void* m_Ptr;
    SdkNativeValueType m_Type;
};

#include <wrapper/utils.h>
#include <algorithm>

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

template <typename T>
inline void PushArgumentAsType(SdkNativeInvoker* nativeInvoker, T&& Value)
{
    auto lookupTable = lua::Runtime::GetInstance()->GetLookupTable();
    lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, &Value, sizeof(T));
}

void PushArg(SdkNativeInvoker* nativeInvoker, lua_State* L, int idx, SdkNativeValueType type)
{
    auto lookupTable = lua::Runtime::GetInstance()->GetLookupTable();

    int luaType = lua_type(L, idx);
    switch (luaType)
    {
        case LUA_TBOOLEAN: PushArgumentAsType(nativeInvoker, lua_toboolean(L, idx)); break;
        case LUA_TNUMBER:
        {
            switch (type)
            {
                case SdkNativeValueType::FLOAT_TYPE: PushArgumentAsType(nativeInvoker, (float)lua_tonumber(L, idx)); break;
                case SdkNativeValueType::INT_TYPE: PushArgumentAsType(nativeInvoker, lua_tointeger(L, idx)); break;
                case SdkNativeValueType::UINT_TYPE: PushArgumentAsType(nativeInvoker, lua_tounsigned(L, idx)); break;
            }
            break;
        }
        case LUA_TVECTOR:
        {
            const float* vector = lua_tovector(L, idx);
            PushArgumentAsType(nativeInvoker, NVector32(vector[0], 0, vector[1], 0, vector[2], 0));
            break;
        }
        case LUA_TSTRING: PushArgumentAsType(nativeInvoker, _strdup(lua_tostring(L, idx))); break;
        default: break;
    }
}

ArgPtr PushArgPtr(SdkNativeInvoker* nativeInvoker, lua_State* L, int idx, SdkNativeValueType type)
{
    auto lookupTable = lua::Runtime::GetInstance()->GetLookupTable();
    void* buffer = nullptr;

    switch (type)
    {
        case SdkNativeValueType::BOOL_TYPE: buffer = CreatePtr<bool>(); break;
        case SdkNativeValueType::FLOAT_TYPE: buffer = CreatePtr<float>(); break;
        case SdkNativeValueType::INT_TYPE: buffer = CreatePtr<int>(); break;
        case SdkNativeValueType::UINT_TYPE: buffer = CreatePtr<unsigned int>(); break;
        case SdkNativeValueType::VECTOR_TYPE: buffer = CreatePtr<NVector32>(); break;
        case SdkNativeValueType::STRING_TYPE: buffer = *CreatePtr<char*>(_strdup(lua_tostring(L, idx))); break;
    }

    switch (type)
    {
        case SdkNativeValueType::BOOL_TYPE: lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, (bool*)&buffer, sizeof(bool*)); break;
        case SdkNativeValueType::FLOAT_TYPE: lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, (float*)&buffer, sizeof(float*)); break;
        case SdkNativeValueType::INT_TYPE: lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, (int*)&buffer, sizeof(int*)); break;
        case SdkNativeValueType::UINT_TYPE: lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, (unsigned int*)&buffer, sizeof(unsigned int*)); break;
        case SdkNativeValueType::VECTOR_TYPE: lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, (NVector32*)&buffer, sizeof(NVector32*)); break;
        case SdkNativeValueType::STRING_TYPE: lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, (char*)&buffer, sizeof(char*)); break;
    }

    return ArgPtr{ buffer, type };
}

void PushValue(SdkNativeInvoker* nativeInvoker, lua_State* L, SdkNativeValueType type)
{

    auto lookupTable = lua::Runtime::GetInstance()->GetLookupTable();
    switch (type)
    {
        case SdkNativeValueType::BOOL_TYPE: lua_pushboolean(L, *(bool*)lookupTable->SdkNativeInvoker__GetReturnValueBuffer(nativeInvoker)); break;
        case SdkNativeValueType::FLOAT_TYPE: lua_pushnumber(L, *(float*)lookupTable->SdkNativeInvoker__GetReturnValueBuffer(nativeInvoker)); break;
        case SdkNativeValueType::INT_TYPE: lua_pushinteger(L, (*(int*)lookupTable->SdkNativeInvoker__GetReturnValueBuffer(nativeInvoker))); break;
        case SdkNativeValueType::UINT_TYPE: lua_pushunsigned(L, *(uint32_t*)lookupTable->SdkNativeInvoker__GetReturnValueBuffer(nativeInvoker)); break;
        case SdkNativeValueType::STRING_TYPE: lua_pushstring(L, *(const char**)lookupTable->SdkNativeInvoker__GetReturnValueBuffer(nativeInvoker)); break;
        case SdkNativeValueType::VECTOR_TYPE:
        {
            NVector32 vector = *(NVector32*)lookupTable->SdkNativeInvoker__GetReturnValueBuffer(nativeInvoker);
            lua_pushvector(L, vector.x, vector.y, vector.z);
            break;
        }
    }
}

void PushValuePtr(SdkNativeInvoker* nativeInvoker, lua_State* L, SdkNativeValueType type, void* buffer)
{
    auto lookupTable = lua::Runtime::GetInstance()->GetLookupTable();
    switch (type)
    {
        case SdkNativeValueType::BOOL_TYPE: lua_pushboolean(L, *(bool*)buffer); break;
        case SdkNativeValueType::FLOAT_TYPE: lua_pushnumber(L, *(float*)buffer); break;
        case SdkNativeValueType::INT_TYPE: lua_pushinteger(L, *(int*)buffer); break;
        case SdkNativeValueType::UINT_TYPE: lua_pushunsigned(L, *(unsigned int*)buffer); break;
        case SdkNativeValueType::STRING_TYPE: lua_pushstring(L, *(const char**)buffer); break;
        case SdkNativeValueType::VECTOR_TYPE:
        {
            NVector32 vector = *(NVector32*)buffer;
            lua_pushvector(L, vector.x, vector.y, vector.z);
            break;
        }
    }
}

int InvokeNative(lua_State* L)
{
    SdkNativeInformation* nativeInformation = reinterpret_cast<SdkNativeInformation*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (nativeInformation == nullptr)
        return 0;

    //uint64_t nativeHash = (uint64_t(hi) << 32) | lo;

    auto lookupTable = lua::Runtime::GetInstance()->GetLookupTable();
    SdkNativeInvoker* invokerFactory = lookupTable->SdkNativeInvoker__Create();

    std::vector<ArgPtr> m_ReturnPointers;
    if (invokerFactory)
    {
        lookupTable->SdkNativeInvoker__Begin(invokerFactory, nativeInformation->m_Hash);

        int ptrArgumentCount = std::count_if(
                nativeInformation->m_ParameterValueArrayData,
                nativeInformation->m_ParameterValueArrayData + nativeInformation->m_ParameterValueArraySize,
                [](const SdkNativeValueInformation& information)
                {
                    return information.m_IsPointer;
                });

        /*if (ptrArgumentCount)
            return 0;*/

        int skippingArgPtr = 0;
        for (int i = 1; i <= lua_gettop(L); i++)
        {
            SdkNativeValueInformation nativeParameter = nativeInformation->m_ParameterValueArrayData[i - 1];
            bool isPointer = nativeParameter.m_IsPointer;

            if (!isPointer)
                PushArg(invokerFactory, L, i, nativeParameter.m_Type);
            else
            {
                PushArgPtr(invokerFactory, L, i, nativeParameter.m_Type);
                skippingArgPtr++;
            }
        }

        for (int x = skippingArgPtr; x < nativeInformation->m_ParameterValueArraySize; x++)
        {
            auto nativeParameter = nativeInformation->m_ParameterValueArrayData[x];
            if(nativeInformation->m_ParameterValueArrayData[x].m_IsPointer)
                m_ReturnPointers.push_back(PushArgPtr(invokerFactory, L, x, nativeParameter.m_Type));
        }

        lookupTable->SdkNativeInvoker__Call(invokerFactory);
        //if (nativeInformation->m_ReturnValue.m_Type != sdk::NativeValueType::Void)
        {
            PushValue(invokerFactory, L, nativeInformation->m_ReturnValue.m_Type);
            for (auto& ret : m_ReturnPointers)
            {
                PushValuePtr(invokerFactory, L, ret.m_Type, ret.m_Ptr);
                delete ret.m_Ptr;
            }

            int defaultReturn = nativeInformation->m_ReturnValue.m_Type != SdkNativeValueType::VOID_TYPE;
            return defaultReturn + m_ReturnPointers.size();
        }
    }

    return 0;
}

void lua::Definitions::Native::Initialize()
{
    m_State->GetGlobalNamespace();
    {
        m_State->BeginNamespace("native");
        {
            auto nativeReflection = m_LookupTable->SdkNativeReflection__Create();
            int32_t nativeSize;

            uint64_t* nativeData = nullptr;
            m_LookupTable->SdkNativeReflection__GetListOfNatives(nativeReflection, &nativeData, &nativeSize);
            for(int i = 0; i < nativeSize; i++)
            {
                SdkNativeInformation* nativeInformation = m_LookupTable->SdkNativeReflection__GetNativeInformation(nativeReflection, nativeData[i]);
                if (nativeInformation == nullptr)
                    continue;

                lua_pushlightuserdata(m_State->GetState(), nativeInformation);
                m_State->RegisterCFunction(nativeInformation->m_Name, InvokeNative, 1);
            }
            delete nativeData;
        }
        m_State->EndNamespace();
    }
    m_State->EndNamespace();
}