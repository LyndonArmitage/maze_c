#ifndef MAZE_SIDEWINDER_H
#define MAZE_SIDEWINDER_H
#include "../Maze.h"
#include "../utils.h"

void generate_sidewinder_maze(Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator");
        exit(EXIT_FAILURE);
    }
    int width = maze->width, height = maze->height;
    unlink_all_cells(maze);

    CellListEntry *run_of_cells = NULL;
    bool new_run = true;
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            if (y == 0) {
                link_cell_in_dir(maze, cell, EAST);
            } else {
                if (new_run) {
                    run_of_cells = new_cell_list_entry(cell);
                    new_run = false;
                } else {
                    push_to_cell_list(run_of_cells, cell);
                }
                bool should_east = coin_flip();
                if (should_east && x < width-1) {
                    link_cell_in_dir(maze, cell, EAST);
                } else {
                    Cell *random = pick_from_cell_list(run_of_cells);
                    link_cell_in_dir(maze, random, NORTH);
                    delete_cell_list(run_of_cells);
                    run_of_cells = NULL;
                    new_run = true;
                }
            }
        }
    }
    if(run_of_cells != NULL) {
        free(run_of_cells);
    }

}

#endif //MAZE_SIDEWINDER_H
