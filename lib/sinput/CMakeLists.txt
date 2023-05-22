cmake_minimum_required (VERSION 3.8)

project ("sinput" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("sinput" src/sinput.c)

# require SDL2 as a peer dependency
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})