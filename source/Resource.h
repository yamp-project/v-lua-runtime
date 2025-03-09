#ifndef SOURCE_RESOURCE_H
#define SOURCE_RESOURCE_H

#include <filesystem>
#include <string>

#include <v-sdk/resource.h>

namespace lua
{
    class Runtime;
    class Resource
    {
        friend class Runtime;

    public:
        void OnStart();
        void OnStop();
        void OnTick();

    private:
        Resource(SdkResource *resource);
        ~Resource() = default;

        Runtime* m_Runtime = nullptr;
        SdkResource* m_Resource = nullptr;
    };
}

#endif //SOURCE_RESOURCE_H
