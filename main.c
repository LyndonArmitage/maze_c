#include <stdio.h>
#include <time.h>
#include "Maze.h"
#include "generator/BinaryTree.h"
#include "generator/Sidewinder.h"
#include "generator/Aldous_Broder.h"
#include "generator/HuntKill.h"
#include "generator/BSP.h"
#include "generator/example.h"
#include "SDL_Maze_Renderer.h"
#include "io.h"

int main(int argc, char **args) {

    if (argc < 3) {
        fprintf(stderr, "Missing width and height arguments\n");
        fprintf(stderr, "arg1 is width (required)\n");
        fprintf(stderr, "arg2 is height (required)\n");
        fprintf(stderr, "arg3 is algorithm (optional)\n");
        fprintf(stderr, "arg4 is cell-size (optional)\n");
        return EXIT_FAILURE;
    }

    const int width = (int) strtol(args[1], NULL, 10);
    const int height = (int) strtol(args[2], NULL, 10);

    if (width < 0) {
        fprintf(stderr, "width must be greater than 0, was %d\n", width);
        return EXIT_FAILURE;
    }
    if (height < 0) {
        fprintf(stderr, "height must be greater than 0, was %d\n", height);
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
        }else if (strcmp(name, "example") == 0) {
            algorithm = generate_example_maze;
        } else {
            fprintf(stderr, "Unknown algorithm: %s\n", name);
            fprintf(
                    stderr,
                    "Valid algorithms are:\naldous,hunt,sidewinder,binary,bsp,example\n"
            );
            return EXIT_FAILURE;
        }
    } else {
        algorithm = generate_hunt_and_kill_maze;
    }

    int cell_size;
    if (argc >= 5) {
        cell_size = (int) strtol(args[4], NULL, 10);
        if (cell_size < 3) {
            fprintf(stderr, "cell-size must be 3+\n");
            return EXIT_FAILURE;
        }
    } else {
        cell_size = 10;
    }

    srand(time(NULL));
    Maze *maze = new_maze(width, height, false);
    render_maze_with_refresh(maze, cell_size, algorithm);
    delete_maze(maze);
    return 0;
}
