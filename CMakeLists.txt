cmake_minimum_required(VERSION 3.7)
project(GG_WD)

set(CMAKE_BUILD_TYPE Debug)

find_package(SDL2 REQUIRED)

include_directories(GG_WD ${SDL2_INCLUDE_DIRS})

add_executable(GG_WD main.c render.c window.c shader.c)

target_link_libraries(GG_WD ${SDL2_LIBRARIES})

target_link_libraries(GG_WD dl)

target_link_libraries(GG_WD m)

target_link_libraries(GG_WD epoxy)



