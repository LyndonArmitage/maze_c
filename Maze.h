#ifndef MAZE_MAZE_H
#define MAZE_MAZE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

typedef struct Cell {
    int x, y;
    unsigned int neighbour_count;
    struct Cell **neighbours;
} Cell;

Cell *new_cell(int x, int y) {
    Cell *cell = malloc(sizeof(Cell));
    cell->x = x;
    cell->y = y;

    // fixed count of potential neighbours in cardinal directions only
    cell->neighbour_count = 4;
    cell->neighbours = malloc(sizeof(Cell *) * cell->neighbour_count);
    for (int i = 0; i < cell->neighbour_count; i++) {
        cell->neighbours[i] = NULL;
    }
    return cell;
}

/**
 * Returns the position of the cell_to_find in the cell_to_search's neighbours
 * array.
 *
 * Will be -1 if not found.
 * @param cell_to_search
 * @param cell_to_find
 * @return -1 if not found otherwise index of cell_to_find in cell_to_search's
 * neighbours array.
 */
int neighbour_pos(Cell *cell_to_search, Cell *cell_to_find) {
    if (cell_to_search == NULL || cell_to_find == NULL) {
        return false;
    }
    for (int i = 0; i < cell_to_search->neighbour_count; i++) {
        Cell *n = cell_to_search->neighbours[i];
        if (n == cell_to_find) {
            return i;
        }
    }
    return -1;
}

void delete_cell(Cell *cell, bool remove_from_neighbours) {
    if (remove_from_neighbours) {
        for (int i = 0; i < cell->neighbour_count; i++) {
            Cell *neighbour = cell->neighbours[i];
            if (neighbour != NULL) {
                int my_pos = neighbour_pos(neighbour, cell);
                if (my_pos != -1) {
                    neighbour->neighbours[my_pos] = NULL;
                }
            }
        }
    }
    free(cell);
}

/**
 * Maze data type.
 *
 * Holds the width and height as well as Cell entries.
 */
typedef struct {
    int width;
    int height;
    Cell **cells;
} Maze;

/**
 * Gets the Cell at the given x,y coordinate
 * @param maze The maze
 * @param x The x location of the cell
 * @param y The y location of the cell
 * @return A pointer to the cell at (x,y) or NULL if there is no entry there
 */
Cell *cell_at(Maze *maze, int x, int y) {
    if (maze == NULL) {
        return NULL;
    }
    int width = maze->width;
    int height = maze->height;
    if (x >= width || y >= height || x < 0 || y < 0) {
        return NULL;
    }
    return maze->cells[(y * width) + x];
}

/**
 * Gets the cell adjacent to the given cell in the given direction.
 *
 * Returns NULL if there is no cell in that direction
 * @param maze The maze
 * @param cell The current cell
 * @param direction The direction to look in
 * @return NULL if there is no cell, otherwise the cell in that direction
 */
Cell *get_cell_adjacent(Maze *maze, Cell *cell, int direction) {
    if (direction < NORTH || direction > WEST || maze == NULL || cell == NULL) {
        return NULL;
    }
    int x = cell->x, y = cell->y;
    switch (direction) {
        case NORTH:
            return cell_at(maze, x, y - 1);
        case EAST:
            return cell_at(maze, x + 1, y);
        case SOUTH:
            return cell_at(maze, x, y + 1);
        case WEST:
            return cell_at(maze, x - 1, y);
        default:
            return NULL;
    }
}


/**
 * Sets all the neighbour values in a cell to those surrounding it in the maze
 * @param maze The maze
 * @param cell The cell to work on
 */
void set_all_neighbouring_cells(Maze *maze, Cell *cell) {
    Cell *north = get_cell_adjacent(maze, cell, NORTH);
    Cell *east = get_cell_adjacent(maze, cell, EAST);
    Cell *south = get_cell_adjacent(maze, cell, SOUTH);
    Cell *west = get_cell_adjacent(maze, cell, WEST);

    cell->neighbours[NORTH] = north;
    cell->neighbours[EAST] = east;
    cell->neighbours[SOUTH] = south;
    cell->neighbours[WEST] = west;
}


void unlink_all_cells(Maze *maze) {
    int width = maze->width;
    int height = maze->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = maze->cells[(y * width) + x];
            for (int i = 0; i < cell->neighbour_count; i++) {
                cell->neighbours[i] = NULL;
            }
        }
    }
}

void link_all_adjacent_cells(Maze *maze) {
    int width = maze->width;
    int height = maze->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = maze->cells[(y * width) + x];
            set_all_neighbouring_cells(maze, cell);
        }
    }
}

Maze *new_maze(int width, int height, bool all_linked) {
    Maze *maze = malloc(sizeof(Maze));

    maze->width = width;
    maze->height = height;
    maze->cells = malloc(sizeof(Cell *) * width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = new_cell(x, y);
            maze->cells[(y * width) + x] = cell;
        }
    }

    if (all_linked) {
        link_all_adjacent_cells(maze);
    }

    return maze;
}


/**
 * Remove a cell from a maze and delete it.
 *
 * Removes the cell from all of it's neighbours as well.
 * @param maze The maze
 * @param x The x location of the cell
 * @param y The y location of the cell
 */
void remove_cell(Maze *maze, int x, int y) {
    int width = maze->width;
    Cell *cell = maze->cells[(y * width) + x];
    if (cell != NULL) {
        delete_cell(cell, true);
        maze->cells[(y * width) + x] = NULL;
    }
}

void delete_maze(Maze *maze) {
    int width = maze->width;
    int height = maze->height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            delete_cell(cell, false);
        }
    }
    free(maze->cells);
    free(maze);
}

void unlink_cells(Cell *cell1, Cell *cell2) {
    if (cell1 == NULL || cell2 == NULL) {
        return;
    }
    int cell2_pos = neighbour_pos(cell1, cell2);
    if (cell2_pos != -1) {
        cell1->neighbours[cell2_pos] = NULL;
    }

    int cell1_pos = neighbour_pos(cell2, cell1);
    if (cell1_pos != -1) {
        cell2->neighbours[cell1_pos] = NULL;
    }
}

void unlink_cell_in_dir(Cell *cell, int dir) {
    if (cell == NULL || dir < NORTH || dir > WEST) {
        return;
    }
    Cell *neighbour = cell->neighbours[dir];
    if (neighbour != NULL) {
        int my_pos = neighbour_pos(neighbour, cell);
        if (my_pos != -1) {
            neighbour->neighbours[my_pos] = NULL;
        }
        cell->neighbours[dir] = NULL;
    }
}

void link_cell_in_dir(Maze *maze, Cell *cell, int dir) {
    if (maze == NULL || cell == NULL || dir < NORTH || dir > WEST) {
        return;
    }
    Cell *neighbour = get_cell_adjacent(maze, cell, dir);
    if (neighbour != NULL) {
        cell->neighbours[dir] = neighbour;
        int my_pos;
        switch (dir) {
            case NORTH:
                my_pos = SOUTH;
                break;
            case EAST:
                my_pos = WEST;
                break;
            case SOUTH:
                my_pos = NORTH;
                break;
            case WEST:
                my_pos = EAST;
                break;
            default:
                my_pos = -1;
        }
        if (my_pos != -1) {
            neighbour->neighbours[my_pos] = cell;
        }
    }
}

/**
 * Link 2 adjacent cells.
 *
 * If the cells are not adjacent they will not be linked
 * @param cell1 First cell
 * @param cell2 Second cell
 */
void link_adjacent_cells(Cell *cell1, Cell *cell2) {
    if (cell1 == NULL || cell2 == NULL || cell1 == cell2) {
        return;
    }

    int x_diff = cell1->x - cell2->x;
    int y_diff = cell1->y - cell2->y;

    int abs_x = abs(x_diff);
    int abs_y = abs(y_diff);
    if (abs_x > 1 || abs_y > 1 || (abs_x == 1 && abs_y == 1)) {
        // these cells are not next to each other, or are diagonals
        return;
    }

    int cell1_dir;
    int cell2_dir;
    if (x_diff != 0) {
        if (x_diff > 0) {
            cell1_dir = WEST;
            cell2_dir = EAST;
        } else {
            cell1_dir = EAST;
            cell2_dir = WEST;
        }
    } else if (y_diff != 0) {
        if (y_diff > 0) {
            cell1_dir = NORTH;;
            cell2_dir = SOUTH;;
        } else {
            cell1_dir = SOUTH;
            cell2_dir = NORTH;
        }
    } else {
        return;
    }

    cell1->neighbours[cell1_dir] = cell2;
    cell2->neighbours[cell2_dir] = cell1;
}

typedef struct {
    bool north, east, south, west;
} Directions;

Directions get_unblocked_directions(Cell *cell) {
    Directions dirs;
    if (cell == NULL) {
        dirs.north = false;
        dirs.east = false;
        dirs.south = false;
        dirs.west = false;
    } else {
        dirs.north = cell->neighbours[NORTH];
        dirs.east = cell->neighbours[EAST];
        dirs.south = cell->neighbours[SOUTH];
        dirs.west = cell->neighbours[WEST];
    }
    return dirs;
}

Directions get_blocked_directions(Cell *cell) {
    Directions dirs = get_unblocked_directions(cell);
    dirs.north = !dirs.north;
    dirs.east = !dirs.east;
    dirs.south = !dirs.south;
    dirs.west = !dirs.west;
    return dirs;
}

void print_maze(Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze to print");
        return;
    }
    unsigned int width = maze->width;
    unsigned int height = maze->height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            if (cell == NULL) {
                printf("#");
                continue;
            }
            Directions dirs = get_unblocked_directions(cell);
            if (dirs.north && dirs.east && dirs.south && dirs.west) {
                printf("╬");
            } else if (dirs.north && dirs.east && dirs.south) {
                printf("╠");
            } else if (dirs.north && dirs.west && dirs.south) {
                printf("╣");
            } else if (dirs.north && dirs.east && dirs.west) {
                printf("╩");
            } else if (dirs.south && dirs.east && dirs.west) {
                printf("╦");
            } else if (dirs.north && dirs.east) {
                printf("╚");
            } else if (dirs.north && dirs.west) {
                printf("╝");
            } else if (dirs.north && dirs.south) {
                printf("║");
            } else if (dirs.east && dirs.west) {
                printf("═");
            } else if (dirs.south && dirs.west) {
                printf("╗");
            } else if (dirs.south && dirs.east) {
                printf("╔");
            } else if (dirs.north) {
                printf("╨");
            } else if (dirs.south) {
                printf("╥");
            } else if (dirs.east) {
                printf("╞");
            } else if (dirs.west) {
                printf("╡");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

#endif //MAZE_MAZE_H
