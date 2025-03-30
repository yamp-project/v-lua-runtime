#pragma once

extern "C"
{
#include <c-sdk/lookup_table.h>
}

namespace lua
{
    class Logger
    {
    public:
        static Logger* Get(const char* name)
        {
            if(m_LookupTable)
                return new Logger(m_LookupTable->SdkLogger__Create(name));

            return new Logger(nullptr);
        }

#define IMPLEMENT_LOGGER(logFunction) \
        template<typename... Args> \
        void logFunction(const char* fmt, Args&&... args) \
        {                             \
            if(m_LookupTable && m_Logger)                          \
                m_LookupTable->SdkLogger__##logFunction(m_Logger, fmt, std::forward<Args>(args)...); \
            else                      \
                printf(fmt, std::forward<Args>(args)...);\
        }

        IMPLEMENT_LOGGER(Info);
        IMPLEMENT_LOGGER(Debug);
        IMPLEMENT_LOGGER(Warning);
        IMPLEMENT_LOGGER(Error);
        IMPLEMENT_LOGGER(Trace);

        static void Initialise(SdkLookupTable* lookupTable)
        {
            if(!m_LookupTable)
                m_LookupTable = lookupTable;
        }

    private:
        Logger(SdkLogger* logger) : m_Logger(logger) { };

        SdkLogger* m_Logger = nullptr;
        inline static SdkLookupTable* m_LookupTable = nullptr;
    };
}