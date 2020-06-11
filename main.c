#include <stdio.h>
#include "Maze.h"

int main() {
    srand(32);
    Maze *maze = new_maze(5, 5, false);

    int width = maze->width;
    int height = maze->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            int dir;
            if ((rand() % 100) < 50) {
                dir = NORTH;
            } else {
                dir = EAST;
            }
            link_cell_in_dir(maze, cell, dir);
        }
    }

    print_maze(maze);
    delete_maze(maze);
    return 0;
}
