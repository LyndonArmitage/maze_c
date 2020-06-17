#include <stdio.h>
#include <time.h>
#include "Maze.h"
#include "generator/BinaryTree.h"
#include "generator/Sidewinder.h"
#include "generator/Aldous_Broder.h"
#include "generator/HuntKill.h"
#include "generator/BSP.h"
#include "SDL_Maze_Renderer.h"

int main(int argc, char **args) {

    int width;
    if (argc >= 2) {
        width = (int) strtol(args[1], NULL, 10);
    } else {
        width = 50;
    }
    int height;
    if (argc >= 3) {
        height = (int) strtol(args[2], NULL, 10);
    } else {
        height = 50;
    }

    srand(time(NULL));
    Maze *maze = new_maze(width, height, false);
    render_maze_with_refresh(maze, 10, generate_BSP_maze);
    delete_maze(maze);
    return 0;
}
