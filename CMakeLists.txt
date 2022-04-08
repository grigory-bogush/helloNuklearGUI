cmake_minimum_required(VERSION 2.8)

project(hello_nuklear)

find_package(SDL2 REQUIRED)
include_directories(main ${SDL2_INCLUDE_DIRS})

set(SOURCE_EXE ./src/main.c)

add_executable(main ${SOURCE_EXE})
target_link_libraries(main ${SDL2_LIBRARIES})
target_link_libraries(main m)