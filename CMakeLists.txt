cmake_minimum_required(VERSION 3.17)
project(maze C)

set(CMAKE_C_STANDARD 11)

add_executable(maze main.c Maze.h generator/BinaryTree.h SDL_Maze_Renderer.h utils.h generator/Sidewinder.h generator/Aldous_Broder.h generator/HuntKill.h generator/BSP.h generator/example.h io.h)

find_package(SDL2 REQUIRED)
target_link_libraries(maze SDL2)