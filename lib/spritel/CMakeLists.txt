cmake_minimum_required (VERSION 3.8)

project ("spritel" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("spritel" src/spritel.c)

if( NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    # peer dependencies
    # log a message
    message(STATUS "spritel: SDL2 and SDL2_image are required to build the library")
    find_package(SDL2 REQUIRED)
    find_package(sdl2-image REQUIRED)
else()
message(STATUS "spritel: SDL2 and SDL2_image are assumed to be handled by emscripten")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})