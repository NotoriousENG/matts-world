cmake_minimum_required (VERSION 3.8)

project ("sinput" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("sinput" src/sinput.c)

if( NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    # peer dependencies
    # log a message
    message(STATUS "sinput: SDL2 is required to build the library")
    find_package(SDL2 REQUIRED)
else()
message(STATUS "sinput: SDL2 is assumed to be handled by emscripten")
endif()

# require SDL2 as a peer dependency
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})