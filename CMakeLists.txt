﻿# CMakeList.txt : CMake project for TinksWorld, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MattsWorld" C)

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# print the current cmake build environment
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    add_compile_definitions("EMSCRIPTEN")
    set(USE_FLAGS "-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2 --preload-file assets --use-preload-plugins")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
    find_package(SDL2 REQUIRED)
    find_package(sdl2-mixer REQUIRED)
    find_package(sdl2-image REQUIRED)
    find_package(sdl2-ttf REQUIRED)
endif()

# Add library subdirectories
add_subdirectory(lib/clm)
add_subdirectory(lib/chroma)
add_subdirectory(lib/sinput)
add_subdirectory(lib/spritel)
add_subdirectory(lib/cute)
add_subdirectory(lib/oof)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "src/main.c" "src/scene.c" "src/resources.c" "src/entity.c" "src/metadata.c" "src/dialogue.c")

# if this is a release build #define DEBUG_COLLISIONS 1
if (NOT CMAKE_BUILD_TYPE MATCHES Release) # append AND false to disable in debug
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_COLLISIONS=1)
endif()


# define GAME_NAME as Matt's World
target_compile_definitions(${PROJECT_NAME} PRIVATE GAME_NAME="Matt's World")

# self published libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/clm/include)
target_link_libraries(${PROJECT_NAME} PRIVATE clm)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/chroma/include)
target_link_libraries(${PROJECT_NAME} PRIVATE chroma)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/sinput/include)
target_link_libraries(${PROJECT_NAME} PRIVATE sinput)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/spritel/include)
target_link_libraries(${PROJECT_NAME} PRIVATE spritel)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/cute/include)
target_link_libraries(${PROJECT_NAME} PRIVATE cute)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/oof/include)
target_link_libraries(${PROJECT_NAME} PRIVATE oof)

# project includes
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include")

# dependencies
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_LIBRARIES}) # ensure sdl2-mixer "extensions" are installed for .ogg: https://www.reddit.com/r/cataclysmdda/comments/glxgtb/fix_for_sound_problem_when_compiling_in_windows/
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_TTF_LIBRARIES})

if (WIN32)
    # /ENTRY:mainCRTStartup keeps the same "main" function instead of requiring "WinMain"
    # if release
    if (CMAKE_BUILD_TYPE MATCHES Release)
        set(SUBSYSTEM_LINKER_OPTIONS "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup" "/LTCG")
    endif()
    target_link_options(${PROJECT_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})

    add_custom_target(copy_intermediate_assets
        # copy tiled directory from list dir's intermediate/tiled to the list dir's assets/tiles
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/intermediate/tiled ${CMAKE_CURRENT_LIST_DIR}/assets/tiles
    )
    add_dependencies(${PROJECT_NAME} copy_intermediate_assets) #fails on windows because the files are locked

    # windows cmake puts into the build type dir
    add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assets
    )
    add_dependencies(${PROJECT_NAME} copy_assets)
else()
    add_custom_target(copy_intermediate_assets
        # copy all *.tmj and *.png files from list dir's intermediate/tiled to the list dir's assets/tiles, do not copy anything else
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/intermediate/tiled/*.tmj ${CMAKE_CURRENT_LIST_DIR}/assets/tiles
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/intermediate/tiled/*.png ${CMAKE_CURRENT_LIST_DIR}/assets/tiles
    )
    add_dependencies(${PROJECT_NAME} copy_intermediate_assets) #fails on windows because the files are locked

    add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    )
    add_dependencies(${PROJECT_NAME} copy_assets)
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    add_custom_target(copy_html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/web/index.html ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.html
        # replace "index.js" in ${PROJECT_NAME}.html with "${PROJECT_NAME}.js"
        COMMAND sed -i "s/index.js/${PROJECT_NAME}.js/g" ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.html
    )
    add_dependencies(${PROJECT_NAME} copy_html)
endif()