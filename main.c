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

    if (width < 0) {
        fprintf(stderr, "width must be greater than 0, was %d", width);
        return EXIT_FAILURE;
    }
    if (height < 0) {
        fprintf(stderr, "height must be greater than 0, was %d", height);
        return EXIT_FAILURE;
    }

    void (*algorithm)(const Maze *);
    if (argc >= 4) {
        char *name = args[3];
        if (strcmp(name, "aldous") == 0) {
            algorithm = generate_aldous_broder_maze;
        } else if (strcmp(name, "hunt") == 0 || strcmp(name, "kill") == 0 || strcmp(name, "huntkill") == 0) {
            algorithm = generate_hunt_and_kill_maze;
        } else if (strcmp(name, "sidewinder") == 0) {
            algorithm = generate_sidewinder_maze;
        } else if (strcmp(name, "binary") == 0 || strcmp(name, "tree") == 0) {
            algorithm = generate_binary_tree_maze;
        } else if (strcmp(name, "bsp") == 0) {
            algorithm = generate_BSP_maze;
        } else {
            fprintf(stderr, "Unknown algorithm: %s", name);
            return EXIT_FAILURE;
        }
    } else {
        algorithm = generate_hunt_and_kill_maze;
    }

    const int cell_size = 10;

    srand(time(NULL));
    Maze *maze = new_maze(width, height, false);
    render_maze_with_refresh(maze, cell_size, algorithm);
    delete_maze(maze);
    return 0;
}
