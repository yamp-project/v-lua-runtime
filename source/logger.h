#pragma once

#include <yamp-sdk/sdk.h>

#include <stdarg.h>
#include <sstream>
#include <format>

#define IMPLEMENT_LOG(name)                                                                                                                 \
    template <typename... Args>                                                                                                             \
    void name(std::string_view fmt, Args&&... args)                                                                                         \
    {                                                                                                                                       \
        m_Sdk->Log##name(m_Prefix.empty() ? fmt.data() : std::format("{} {}", m_Prefix, fmt).c_str(), std::forward<Args>(args)...); \
    }

class Logger
{
public:

    Logger(SDK_Interface* sdk): m_Sdk(sdk)
    {
        //
    }

    Logger(SDK_Interface* sdk, const std::string& prefix): m_Sdk(sdk), m_Prefix(prefix)
    {
        //
    }

    IMPLEMENT_LOG(Debug);
    IMPLEMENT_LOG(Info);
    IMPLEMENT_LOG(Warn);
    IMPLEMENT_LOG(Error);

private:
    SDK_Interface* m_Sdk;
    std::string m_Prefix;
};
