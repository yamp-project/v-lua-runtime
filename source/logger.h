#pragma once

#include <yamp-sdk/sdk.h>

#include <stdarg.h>
#include <sstream>
#include <format>

#define IMPLEMENT_LOG(name)                                                                                                                 \
    template <typename... Args>                                                                                                             \
    void name(std::string_view fmt, Args&&... args)                                                                                         \
    {                                                                                                                                       \
        m_LookupTable->Log##name(m_Prefix.empty() ? fmt.data() : std::format("{} {}", m_Prefix, fmt).c_str(), std::forward<Args>(args)...); \
    }

class Logger
{
public:

    Logger(ILookupTable* lookupTable): m_LookupTable(lookupTable)
    {
        //
    }

    Logger(ILookupTable* lookupTable, const std::string& prefix): m_LookupTable(lookupTable), m_Prefix(prefix)
    {
        //
    }

    IMPLEMENT_LOG(Debug);
    IMPLEMENT_LOG(Info);
    IMPLEMENT_LOG(Warn);
    IMPLEMENT_LOG(Error);

private:
    ILookupTable* m_LookupTable;
    std::string m_Prefix;
};
