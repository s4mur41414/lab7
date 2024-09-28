cmake_minimum_required(VERSION 3.12)
project(
    labwork7
    VERSION 1.0
    DESCRIPTION "C++ Year 2024 labwork7"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

add_subdirectory(lib)
add_subdirectory(bin)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
