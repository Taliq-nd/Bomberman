cmake_minimum_required(VERSION 3.15)
project(bomberman C)

set(CMAKE_C_STANDARD 99)

find_package(raylib REQUIRED)

file(GLOB SOURCES "src/*.c")

add_executable(bomberman ${SOURCES})
target_include_directories(bomberman PRIVATE src)
target_link_libraries(bomberman raylib)

if(WIN32)
    target_link_libraries(bomberman opengl32 gdi32 winmm)
endif()
