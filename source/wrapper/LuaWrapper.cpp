#include <wrapper/LuaWrapper.h>

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace lua;
State::State()
{
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

void State::RunFile(std::string filePathStr)
{
    std::filesystem::path filePath(filePathStr);

    if(!std::filesystem::exists(filePath))
    {
        printf("File was not found: %s\n", filePathStr.c_str());
        return;
    }

    printf("Main file exists! %s\n", filePath.string().c_str());
    std::ifstream mainFileStream(filePath);

    std::string content((std::istreambuf_iterator<char>(mainFileStream)), (std::istreambuf_iterator<char>()));
    printf("Content: %lld\n", content.size());

    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(content.c_str(), content.size(), NULL, &bytecodeSize);
    int result = luau_load(m_State, "test", bytecode, bytecodeSize, 0);
    free(bytecode);

    lua_pcall(m_State, 0, 0, 0);

    printf("Result: %d\n", result);
}