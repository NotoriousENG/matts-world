cmake_minimum_required (VERSION 3.8)

project ("oof" C)
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
add_library("oof" src/oof.c)