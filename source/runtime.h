#ifndef SOURCE_RUNTIME_H
#define SOURCE_RUNTIME_H

#include <vector>

extern "C"
{
#include <c-sdk/resource.h>
#include <c-sdk/lookup_table.h>
};

#include "logger.h"

namespace lua
{
    class Resource;
    class Runtime
    {
    public:
        static Runtime* GetInstance()
        {
            static Runtime runtimeInstance;
            return &runtimeInstance;
        }

        void Initialize(SdkLookupTable* lookupTable)
        {
            SetLookupTable(lookupTable);
            m_Logger = lua::Logger::Get("lua::Runtime");
        }

        void SetLookupTable(SdkLookupTable* lookupTable)
        {
            m_LookupTable = lookupTable;
        }

        SdkLookupTable* GetLookupTable() { return m_LookupTable; }

        void OnStart();
        void OnStop();
        void OnTick();

        bool OnResourceStart(SdkResource* resource);
        bool OnResourceStop(SdkResource* resource);
        void OnResourceTick(SdkResource* resource);

    private:
        Runtime();
        ~Runtime() = default;

        lua::Logger* m_Logger = nullptr;

        SdkLookupTable* m_LookupTable = nullptr;
        std::vector<lua::Resource*> m_Resources;
    };
}

#endif //SOURCE_RUNTIME_H
