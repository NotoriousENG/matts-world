cmake_minimum_required (VERSION 3.8)

project ("flecs" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("flecs" src/flecs.c)
