#ifndef MAZE_ALDOUS_BRODER_H
#define MAZE_ALDOUS_BRODER_H

#include "../Maze.h"
#include "../utils.h"

void generate_aldous_broder_maze(Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator");
        exit(EXIT_FAILURE);
    }
    int width = maze->width, height = maze->height;
    unlink_all_cells(maze);

    Cell *visited[height][width];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            visited[y][x] = NULL;
        }
    }

    Cell *current = random_cell(maze);
    if (current == NULL) {
        fprintf(stderr, "Unable to start at random cell");
        exit(EXIT_FAILURE);
    }

    int visited_count = 1;
    visited[current->y][current->x] = current;

    int all_cells_count = width * height;
    while (visited_count < all_cells_count) {
        Cell *adjacent = NULL;
        int dir;
        while (adjacent == NULL) {
            dir = random_direction();
            adjacent = get_cell_adjacent(maze, current, dir);
        }
        if (visited[adjacent->y][adjacent->x] == NULL) {
            link_cell_in_dir(maze, current, dir);
            visited[adjacent->y][adjacent->x] = adjacent;
            visited_count ++;
        }
        current = adjacent;
    }
}


#endif //MAZE_ALDOUS_BRODER_H
