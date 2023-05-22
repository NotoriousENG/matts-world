cmake_minimum_required (VERSION 3.8)

project ("spritel" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("spritel" src/spritel.c)

# peer dependencies
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

find_package(sdl2-image REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})