#ifndef MAZE_IO_H
#define MAZE_IO_H

#include <stdio.h>
#include "Maze.h"

/**
 * Write maze to a binary file
 * @param file The file to write to
 * @param maze The maze to write
 * @return 0 if successful
 */
int write_maze(FILE *file, const Maze *maze);

/**
 * Read a maze from a binary file
 * @param file The file to read the maze from
 * @return A pointer to the new maze if successful otherwise NULL
 */
Maze *read_maze(FILE *file);

/**
 * Pack a cell into a single byte.
 *
 * A packed cell basically says which directions can be traversed in like so:
 *
 * <code><pre>
 * ---D WSEN<br/>
 * 0000 0000
 * </pre></code>
 *
 * The 5th bit is set when the Cell is NULL.
 *
 * @param cell The cell to pack
 * @return a byte representing the cell
 */
unsigned char pack_cell(const Cell *cell);

int write_maze(FILE *file, const Maze *maze) {
    if (file == NULL) {
        fprintf(stderr, "No file provided to write to");
        exit(EXIT_FAILURE);
    }
    if (maze == NULL) {
        fprintf(stderr, "No maze provided to write");
        exit(EXIT_FAILURE);
    }

    const int width = maze->width;
    const int height = maze->height;

    // pack the maze into a small format

    // Format will be:
    // MAZE (in ASCII)
    // width integer
    // height integer
    // maze data
    fputs("MAZE", file);
    putw(width, file);
    putw(height, file);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            unsigned char packed_dirs = pack_cell(cell);
            putc(packed_dirs, file);
        }
    }
    return fflush(file);
}

unsigned char pack_cell(const Cell *cell) {
    if (cell == NULL) return 16;
    Directions unblocked = get_unblocked_directions(cell);
    // pack into a byte:
    //      WSEN
    // 0000 0000
    // if the 5th bit is set then this cell was null

    unsigned char packed = 0u;
    if (unblocked.north) packed |= 1u;
    if (unblocked.east) packed |= 2u;
    if (unblocked.south) packed |= 4u;
    if (unblocked.west) packed |= 8u;
    return packed;
}

Maze *read_maze(FILE *file) {
    if (file == NULL) return NULL;

    // Check for MAZE
    char magic[5];
    for (int i = 0; i < 4; i++) {
        int c = fgetc(file);
        if (feof(file) != 0) {
            fprintf(stderr, "Failed to find magic letter in file\n");
            return NULL;
        }
        magic[i] = (char) c;
    }
    magic[4] = 0;
    if (strcmp(magic, "MAZE") != 0) {
        fprintf(stderr, "Not a valid maze file\n");
        return NULL;
    }

    // Get dimensions
    int width;
    int height;
    if (fread(&width, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Could not read width\n");
        return NULL;
    }
    if (fread(&height, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "Could not read height\n");
        return NULL;
    }

    // Load maze
    Maze *maze = new_maze(width, height, false);
    const int size = width * height;

    for (int i = 0; i < size; i++) {
        unsigned char byte = fgetc(file);
        if (feof(file) != 0) {
            fprintf(stderr, "missing cells in file %d/%d", i, size);
            break;
        }
        bool north, east, south, west, delete;
        north = byte & 1u;
        east = byte & 2u;
        south = byte & 4u;
        west = byte & 8u;
        delete = byte & 16u;

        int x = i % width;
        int y = i / width;
        if (delete) {
            remove_cell(maze, x, y);
            continue;
        }

        Cell *cell = cell_at(maze, x, y);
        if (cell == NULL) continue;
        if (north) link_cell_in_dir(maze, cell, NORTH);
        if (east) link_cell_in_dir(maze, cell, EAST);
        if (south) link_cell_in_dir(maze, cell, SOUTH);
        if (west) link_cell_in_dir(maze, cell, WEST);
    }

    return maze;
}

#endif //MAZE_IO_H
