#pragma once

#include <vector>
#include <assert.h>

#include "logger.h"

namespace lua
{
    bool Init();
    void Shutdown();

    void OnResourceStart(IResource* resource);
    void OnResourceStop(IResource* resource);
    void OnTick();
    void OnEvent(void* event);

    class Resource;
    class Runtime
    {
    public:
        // TODO: opt-in for a more dynamic singleton style
        static Runtime* GetInstance()
        {
            static Runtime runtimeInstance;
            return &runtimeInstance;
        }

        Runtime(): m_LookupTable(nullptr), m_Resources()
        {
            //
        }

        void SetLookupTable(ILookupTable* lookupTable)
        {
            assert(m_LookupTable == nullptr);
            m_LookupTable = lookupTable;
            m_Logger = std::make_unique<Logger>(m_LookupTable, "lua");
        }

        ILookupTable* GetLookupTable() {
            assert(m_LookupTable != nullptr);
            return m_LookupTable;
        }

        Logger* GetLogger()
        {
            return m_Logger.get();
        }

    private:
        ~Runtime() = default;

        ILookupTable* m_LookupTable;

        std::vector<Resource*> m_Resources;
        std::unique_ptr<Logger> m_Logger;
    };
}
