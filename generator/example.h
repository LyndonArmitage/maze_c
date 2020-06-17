#ifndef MAZE_EXAMPLE_H
#define MAZE_EXAMPLE_H

#include "../utils.h"
#include "../Maze.h"

void generate_example_maze(const Maze *maze);

void generate_example_zig_zag_maze(const Maze *maze);

void generate_example_spiral_maze(const Maze *maze);

void noop_generate_maze(const Maze *maze);

void generate_example_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator\n");
        exit(EXIT_FAILURE);
    }

    if (coin_flip()) {
        generate_example_spiral_maze(maze);
    } else {
        generate_example_zig_zag_maze(maze);
    }
}

void generate_example_zig_zag_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator\n");
        exit(EXIT_FAILURE);
    }

    const int width = maze->width;
    const int height = maze->height;
    const int total_cells = maze->cell_count;

    if (total_cells != width * height) {
        fprintf(stderr, "Example generation only works on full grids\n");
        exit(EXIT_FAILURE);
    }

    unlink_all_cells(maze);

    bool right = true;
    for (int y = 0; y < height; y++) {
        if (right) {
            for (int x = 0; x < width; x++) {
                Cell *cell = cell_at(maze, x, y);
                if (cell == NULL) continue;
                int dir = x < width - 1 ? EAST : SOUTH;
                link_cell_in_dir(maze, cell, dir);
            }
        } else {
            for (int x = width - 1; x >= 0; x--) {
                Cell *cell = cell_at(maze, x, y);
                if (cell == NULL) continue;
                int dir = x > 0 ? WEST : SOUTH;
                link_cell_in_dir(maze, cell, dir);
            }
        }
        right = !right;
    }
}

void generate_example_spiral_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator\n");
        exit(EXIT_FAILURE);
    }

    const int width = maze->width;
    const int height = maze->height;
    const int total_cells = maze->cell_count;

    if (total_cells != width * height) {
        fprintf(stderr, "Example generation only works on full grids\n");
        exit(EXIT_FAILURE);
    }

    unlink_all_cells(maze);

    const int min_dimension = width < height ? width : height;
    const int half_size = min_dimension / 2;

    int dir = random_direction();
    int size = 1;
    Cell *cell = cell_at(maze, half_size, half_size);
    while (cell != NULL) {
        for (int i = 0; i < size; i++) {
            link_cell_in_dir(maze, cell, dir);
            cell = get_cell_adjacent(maze, cell, dir);
        }
        dir = rotate_clockwise(dir);
        for (int i = 0; i < size; i++) {
            link_cell_in_dir(maze, cell, dir);
            cell = get_cell_adjacent(maze, cell, dir);
        }
        dir = rotate_clockwise(dir);
        size++;
    }
}

void noop_generate_maze(const Maze *maze) {
}

#endif //MAZE_EXAMPLE_H
