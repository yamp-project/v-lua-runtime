#include <wrapper/lua_wrapper.h>

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace lua;
State::State(std::string resourceName)
{
    m_ResourceName = resourceName;
    // m_Logger = lua::Logger::Get("lua::State");
    m_State = luaL_newstate();
    luaL_openlibs(m_State);
}

State::~State()
{

}

int Call(lua_State* L)
{
    printf("FUCKING CALL!!!!!!\n");

    return 0;
}

#include <Luau/Compiler.h>

void State::RunFile(std::string filePathStr, std::string relativePathStr)
{
    std::filesystem::path basePath(relativePathStr);
    std::filesystem::path filePath(filePathStr);

    std::filesystem::path relativePath = std::filesystem::relative(filePath, basePath);
    if(!std::filesystem::exists(filePath))
    {
        printf("File was not found: %s\n", filePathStr.c_str());
        // m_Logger->Error("File was not found: %s", filePathStr.c_str());
        return;
    }

    // m_Logger->Info("Main file exists! %s", filePath.string().c_str());
    std::ifstream mainFileStream(filePath);

    std::string content((std::istreambuf_iterator<char>(mainFileStream)), (std::istreambuf_iterator<char>()));
    printf("Content: %lld\n%s\n", content.size(), content.c_str());

    size_t bytecodeSize = 0;

    std::string bytecode = Luau::compile(content);
    int result = luau_load(m_State, std::format("@{}:{}", m_ResourceName, relativePath.string()).c_str(), bytecode.c_str(), bytecodeSize, 0);

    if(!result)
    {
        auto status = lua_resume(m_State, NULL, 0);
        if (status != 0)
        {
            if(lua_isstring(m_State, -1)) {
                const char* str = lua_tostring(m_State, -1);
                printf("Error happened while running: %s\n", str);
                // m_Logger->Error("Error happened while running: %s\n", str);
            }
        }
    }

    printf("Result: %d\n", result);
    // m_Logger->Info("Result: %d", result);

    lua_close(m_State);
}