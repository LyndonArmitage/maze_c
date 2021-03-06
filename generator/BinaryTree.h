#ifndef MAZE_BINARYTREE_H
#define MAZE_BINARYTREE_H

#include "../utils.h"
#include "../Maze.h"

void generate_binary_tree_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator");
        exit(EXIT_FAILURE);
    }
    const int width = maze->width, height = maze->height;
    unlink_all_cells(maze);

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            Directions dirs = get_blocked_directions(cell);
            int link_dir;
            if (x == width - 1) {
                link_dir = NORTH;
            } else if (y == 0) {
                link_dir = EAST;
            } else {
                if (dirs.north && dirs.east) {
                    bool should_east = coin_flip();
                    if (should_east) {
                        link_dir = EAST;
                    } else {
                        link_dir = NORTH;
                    }
                } else {
                    if (dirs.north) {
                        link_dir = NORTH;
                    } else if (dirs.east) {
                        link_dir = EAST;
                    } else {
                        link_dir = -1;
                    }
                }
            }
            if (link_dir != -1) {
                link_cell_in_dir(maze, cell, link_dir);
            }
        }
    }

}


#endif //MAZE_BINARYTREE_H
