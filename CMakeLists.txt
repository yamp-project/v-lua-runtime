cmake_minimum_required (VERSION 3.8)

project ("v-lua-runtime")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LUA_BUILD_EXE "Make executable for testing purposes" ON)

message(STATUS LUA_BUILD_EXE)

if(LUA_BUILD_EXE)
    message(STATUS "IT IS FUCKING ON")
    add_definitions(-DLUA_BUILD_EXE)
endif()

if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/linux64/${CMAKE_BUILD_TYPE})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/win64/${CMAKE_BUILD_TYPE})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/win64/${CMAKE_BUILD_TYPE})
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
endif()

set(LUAU_BUILD_CLI OFF)
set(LUAU_BUILD_TESTS OFF)
set(LUAU_BUILD_WEB OFF)
set(LUAU_WERROR OFF)
set(LUAU_STATIC_CRT OFF)
#set(LUAU_EXTERN_C ON)

add_subdirectory("./vendors/v-sdk")
add_subdirectory("./vendors/luau")

file(GLOB_RECURSE PROJECT_SOURCE_FILES "./source/*.h" "./source/*.hpp" "./source/*.cpp")

include_directories("./source")

if(LUA_BUILD_EXE)
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
else()
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_FILES})
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC
        Luau.Compiler Luau.VM
        yamp::sdk
)