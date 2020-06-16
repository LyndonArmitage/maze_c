#ifndef MAZE_HUNTKILL_H
#define MAZE_HUNTKILL_H

#include "../Maze.h"
#include "../utils.h"

void generate_hunt_and_kill_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator");
        exit(EXIT_FAILURE);
    }
    const int width = maze->width;
    const int height = maze->height;
    const int total_cells = maze->cell_count;
    unlink_all_cells(maze);

    Cell *visited[height][width];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            visited[y][x] = NULL;
        }
    }
    // Random current cell
    Cell *current = random_cell(maze);
    int visited_count = 1;
    visited[current->y][current->x] = current;

    while (visited_count < total_cells) {
        if (current == NULL) break;

        // Get next cell that is not visited and not linked to
        int neighbour_count = 0;
        Cell** possible_next = get_all_neighbouring_cells(maze, current, &neighbour_count);
        int possible_next_null_count = 0;
        for (int i = 0; i < neighbour_count; i ++) {
            Cell* possible = possible_next[i];
            if (possible == NULL ||current->neighbours[i] == possible || visited[possible->y][possible->x] == possible) {
                possible_next[i] = NULL;
                possible_next_null_count ++;
            }
        }
        Cell *next = NULL;
        if (possible_next_null_count < neighbour_count) {
            while(next == NULL) {
                next = possible_next[rand() % neighbour_count];
            }
        }

        free(possible_next);
        bool hunt_time = next == NULL;
        if (!hunt_time) {
            // random walk
            int x = next->x;
            int y = next->y;
            if (visited[y][x] != next) {
                visited[y][x] = next;
                visited_count++;
                link_adjacent_cells(current, next);
                current = next;
            }
        }
        if (hunt_time) {
            // hunt
            bool finished = false;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    Cell *this_cell = visited[y][x];
                    if (this_cell == NULL) {
                        this_cell = cell_at(maze, x, y);
                        if (this_cell == NULL) continue;
                        int cells_count = 0;
                        Cell **cells = get_all_neighbouring_cells(maze, this_cell, &cells_count);
                        int null_count = 0;
                        for (int i = 0; i < cells_count; i++) {
                            Cell *neighbour = cells[i];
                            if (neighbour == NULL || visited[neighbour->y][neighbour->x] == NULL) {
                                null_count++;
                                cells[i] = NULL;
                            }
                        }
                        if (null_count >= cells_count) {
                            free(cells);
                            continue; // all neighbours are unvisited
                        }

                        // find visited neighbour
                        Cell *visited_neighbour = NULL;
                        while (visited_neighbour == NULL) {
                            int i = rand() % cells_count;
                            visited_neighbour = cells[i];
                        }
                        link_adjacent_cells(this_cell, visited_neighbour);
                        visited[y][x] = this_cell;
                        visited_count++;
                        current = this_cell;
                        finished = true;
                        free(cells);
                        break;
                    }
                }
                if (finished) {
                    break;
                }
            }

            // Something went wrong with algorithm above
            if (!finished) {
                fprintf(stderr, "Failed to finish hunting properly\n");
                fprintf(stderr, "The following cells were corrupted:\n");
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        Cell *visited_cell = visited[y][x];
                        if(visited_cell != cell_at(maze, x, y)) {
                            fprintf(stderr, "%d,%d\n", x, y);
                        }
                    }
                }
                break;
            }
        }


    }

}

#endif //MAZE_HUNTKILL_H
