cmake_minimum_required(VERSION 3.17)
project(maze C)

set(CMAKE_C_STANDARD 11)

add_executable(maze main.c Maze.h)