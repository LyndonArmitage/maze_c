#ifndef MAZE_MAZE_H
#define MAZE_MAZE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DIRECTION_COUNT 4

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

int rotate_clockwise(const int dir) {
    int new_dir = dir + 1;
    if (new_dir > WEST) new_dir = NORTH;
    return new_dir;
}

int rotate_counter_clockwise(const int dir) {
    int new_dir = dir - 1;
    if (new_dir < NORTH) new_dir = WEST;
    return new_dir;
}

typedef struct Cell {
    int x, y;
    unsigned int neighbour_count;
    struct Cell **neighbours;
} Cell;

/**
 * Creates a new Cell with the given x, y coordinates
 * @param x The x coordinate
 * @param y The y coordinate
 * @return A pointer to the new Cell
 */
Cell *new_cell(int x, int y) {
    Cell *cell = malloc(sizeof(Cell));
    if (cell == NULL) {
        fprintf(stderr, "Unable to create cell: %dx%d", x, y);
        exit(EXIT_FAILURE);
    }
    cell->x = x;
    cell->y = y;

    // fixed count of potential neighbours in cardinal directions only
    cell->neighbour_count = DIRECTION_COUNT;
    cell->neighbours = malloc(sizeof(Cell *) * cell->neighbour_count);
    if (cell->neighbours == NULL) {
        fprintf(stderr, "Unable to allocate space for neighbours on cell %dx%d", x, y);
        free(cell);
        exit(EXIT_FAILURE);
    }
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
int neighbour_pos(const Cell *cell_to_search, const Cell *cell_to_find) {
    if (cell_to_search == NULL || cell_to_find == NULL) {
        return -1;
    }
    for (int i = 0; i < cell_to_search->neighbour_count; i++) {
        Cell *n = cell_to_search->neighbours[i];
        if (n == cell_to_find) {
            return i;
        }
    }
    return -1;
}

/**
 * Deletes the cell
 * @param cell A pointer to the cell to delete
 * @param remove_from_neighbours Whether a pointer to the cell should be
 * removed form it's neighbours
 */
void delete_cell(Cell *cell, const bool remove_from_neighbours) {
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
    cell = NULL;
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
    int cell_count;
} Maze;

/**
 * Gets the Cell at the given x,y coordinate
 * @param maze The maze
 * @param x The x location of the cell
 * @param y The y location of the cell
 * @return A pointer to the cell at (x,y) or NULL if there is no entry there
 */
Cell *cell_at(const Maze *maze, int x, int y) {
    if (maze == NULL) {
        return NULL;
    }
    const int width = maze->width;
    const int height = maze->height;
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
Cell *get_cell_adjacent(const Maze *maze, const Cell *cell, int direction) {
    if (direction < NORTH || direction > WEST || maze == NULL || cell == NULL) {
        return NULL;
    }
    const int x = cell->x, y = cell->y;
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
            fprintf(stderr, "Unhandled direction: %d", direction);
            exit(EXIT_FAILURE);
    }
}


/**
 * Sets all the neighbour values in a cell to those surrounding it in the maze
 * @param maze The maze
 * @param cell The cell to work on
 */
void set_all_neighbouring_cells(const Maze *maze, const Cell *cell) {
    if (maze == NULL || cell == NULL) return;
    Cell *north = get_cell_adjacent(maze, cell, NORTH);
    Cell *east = get_cell_adjacent(maze, cell, EAST);
    Cell *south = get_cell_adjacent(maze, cell, SOUTH);
    Cell *west = get_cell_adjacent(maze, cell, WEST);

    cell->neighbours[NORTH] = north;
    cell->neighbours[EAST] = east;
    cell->neighbours[SOUTH] = south;
    cell->neighbours[WEST] = west;
}

/**
 * Get all neighbouring cells to a given cell
 * @param maze The maze
 * @param cell The cell to get neighbours of
 * @param size A pointer to an integer that will be filled with the size of the
 * returned array
 * @return A pointer to an array of pointers to the neighbouring cells
 */
Cell **get_all_neighbouring_cells(
        const Maze *maze,
        const Cell *cell,
        int *size
) {
    if (maze == NULL || cell == NULL) {
        *size = 0;
        return NULL;
    }
    Cell **array = malloc(sizeof(Cell *) * DIRECTION_COUNT);
    if (array == NULL) {
        fprintf(stderr, "Unable to allocate array");
        exit(EXIT_FAILURE);
    }
    array[NORTH] = get_cell_adjacent(maze, cell, NORTH);
    array[EAST] = get_cell_adjacent(maze, cell, EAST);
    array[SOUTH] = get_cell_adjacent(maze, cell, SOUTH);
    array[WEST] = get_cell_adjacent(maze, cell, WEST);
    *size = DIRECTION_COUNT;
    return array;
}


void unlink_all_cells(const Maze *maze) {
    if (maze == NULL) return;
    const int width = maze->width;
    const int height = maze->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = maze->cells[(y * width) + x];
            if (cell == NULL) continue;
            for (int i = 0; i < cell->neighbour_count; i++) {
                cell->neighbours[i] = NULL;
            }
        }
    }
}

void link_all_adjacent_cells(const Maze *maze) {
    if (maze == NULL) return;
    const int width = maze->width;
    const int height = maze->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = maze->cells[(y * width) + x];
            set_all_neighbouring_cells(maze, cell);
        }
    }
}

Maze *new_maze(int width, int height, bool all_linked) {
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Cannot create a maze with dimensions: %dx%d", width, height);
        exit(EXIT_FAILURE);
    }
    Maze *maze = malloc(sizeof(Maze));
    if (maze == NULL) {
        fprintf(stderr, "Unable to create maze");
        exit(EXIT_FAILURE);
    }

    maze->width = width;
    maze->height = height;
    maze->cells = malloc(sizeof(Cell *) * width * height);
    if (maze->cells == NULL) {
        fprintf(stderr, "Unable to create cells");
        free(maze);
        exit(EXIT_FAILURE);
    }
    maze->cell_count = width * height;

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
    if (maze == NULL) return;
    int width = maze->width;
    Cell *cell = cell_at(maze, x, y);
    if (cell != NULL) {
        delete_cell(cell, true);
        maze->cells[(y * width) + x] = NULL;
        maze->cell_count--;
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
    maze->cell_count = 0;
    free(maze->cells);
    maze->cells = NULL;
    free(maze);
    maze = NULL;
}

void unlink_cells(const Cell *cell1, const Cell *cell2) {
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

void unlink_cell_in_dir(const Cell *cell, int dir) {
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

void link_cell_in_dir(const Maze *maze, const Cell *cell, int dir) {
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
            neighbour->neighbours[my_pos] = (Cell *) cell;
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
void link_adjacent_cells(const Cell *cell1, const Cell *cell2) {
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

    cell1->neighbours[cell1_dir] = (Cell *) cell2;
    cell2->neighbours[cell2_dir] = (Cell *) cell1;
}

typedef struct {
    bool north, east, south, west;
} Directions;

Directions get_unblocked_directions(const Cell *cell) {
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

Directions get_blocked_directions(const Cell *cell) {
    Directions dirs = get_unblocked_directions(cell);
    dirs.north = !dirs.north;
    dirs.east = !dirs.east;
    dirs.south = !dirs.south;
    dirs.west = !dirs.west;
    return dirs;
}

void print_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze to print");
        return;
    }
    const int width = maze->width;
    const int height = maze->height;

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
