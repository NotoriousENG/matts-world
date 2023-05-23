cmake_minimum_required (VERSION 3.8)

project ("cute" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("cute" src/cute_tiled.c)