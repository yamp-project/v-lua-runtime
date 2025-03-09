#include "Runtime.h"
#include "Resource.h"

#include <v-sdk/resource.h>
#include <v-sdk/lookup_table.h>

lua::Runtime::Runtime()
{

}

#pragma pack(push, 1)
typedef struct
{
    float x;
    float __padding_x;
    float y;
    float __padding_y;
    float z;
    float __padding_z;
} PaddedVector3;
#pragma pack(pop)

void lua::Runtime::OnStart()
{
    printf("Runtime::OnStart!!!\n");
}

void lua::Runtime::OnStop()
{

}

void lua::Runtime::OnTick()
{

}

template<typename Type>
void PushArgumentAsType(SdkLookupTable* lookupTable, SdkNativeInvoker* invoker, Type&& value)
{
    lookupTable->SdkNativeInvoker__PushArgumentFromBuffer(invoker, &value, sizeof(Type));
}

bool lua::Runtime::OnResourceStart(SdkResource* resource)
{
    lua::Resource* luaResource = new lua::Resource(resource);

    printf("Runtime::OnResourceStart5!!!!\n");

    //int entity = 2;
    //PaddedVector3 forward, right, up, position;
    //static auto nativeInvoker = m_LookupTable->SdkNativeInvoker__Create();
    //printf("Native invoker2: %p\n", nativeInvoker);
    //m_LookupTable->SdkNativeInvoker__Reset(nativeInvoker);
    //m_LookupTable->SdkNativeInvoker__Begin(nativeInvoker, 0xAAB58D9C726404C0);
    //PushArgumentAsType(m_LookupTable, nativeInvoker, (int)2);
    //PushArgumentAsType(m_LookupTable, nativeInvoker, (PaddedVector3*)&forward);
    //PushArgumentAsType(m_LookupTable, nativeInvoker, (PaddedVector3*)&right);
    //PushArgumentAsType(m_LookupTable, nativeInvoker, (PaddedVector3*)&up);
    //PushArgumentAsType(m_LookupTable, nativeInvoker, (PaddedVector3*)&position);
    ////m_LookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, &forward, sizeof(PaddedVector3*));
    ////m_LookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, &right, sizeof(PaddedVector3*));
    ////m_LookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, &up, sizeof(PaddedVector3*));
    ////m_LookupTable->SdkNativeInvoker__PushArgumentFromBuffer(nativeInvoker, &position, sizeof(PaddedVector3*));
    //m_LookupTable->SdkNativeInvoker__Call(nativeInvoker);

    //printf("Forward: %f - %f - %f\n", forward.x, forward.y, forward.z);
    //printf("Right: %f - %f - %f\n", right.x, right.y, right.z);
    //printf("Up: %f - %f - %f\n", up.x, up.y, up.z);
    //printf("Position: %f - %f - %f\n", position.x, position.y, position.z);

    return true;
}

bool lua::Runtime::OnResourceStop(SdkResource* resource)
{
    return true;
}

void lua::Runtime::OnResourceTick(SdkResource* resource)
{

}
