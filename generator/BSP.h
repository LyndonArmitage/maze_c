#ifndef MAZE_BSP_H
#define MAZE_BSP_H

#include "../utils.h"
#include "../Maze.h"

void generate_BSP_maze(Maze const *maze) {
    // BSP = Binary Space Partition
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator");
        exit(EXIT_FAILURE);
    }

    const int width = maze->width;
    const int height = maze->height;
    const int total_cells = maze->cell_count;

    if (total_cells != width * height) {
        fprintf(stderr, "BSP generation only works on full grids");
        exit(EXIT_FAILURE);
    }

    // We will be splitting the grid and essentially connecting the 2 halves
    // each time until we finish

}

CellListEntry


#endif //MAZE_BSP_H
