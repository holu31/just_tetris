cmake_minimum_required(VERSION 3.0)
project(Tetris)

set(CMAKE_C_COMPILER gcc)

find_package(raylib 3.7 QUIET)

file(GLOB SOURCES src/*.c)

add_executable(Tetris ${SOURCES})

target_link_libraries(Tetris raylib)

#set(CMAKE_C_FLAGS "-s -Os -fdata-sections -ffunction-sections -fipa-pta -Wl,--gc-sections -Wl,-O2 -Wl,--as-needed -Wl,--strip-all")

target_include_directories(Tetris PRIVATE ${raylib_INCLUDE_DIRS})