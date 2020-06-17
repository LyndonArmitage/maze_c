#ifndef MAZE_BSP_H
#define MAZE_BSP_H

#include <math.h>
#include "../utils.h"
#include "../Maze.h"

typedef struct bsp_segment {
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    struct bsp_segment *parent;
    struct bsp_segment *child1;
    struct bsp_segment *child2;
} BSP_Segment;

BSP_Segment *new_bsp_segment(int start_x, int start_y, int end_x, int end_y);

void delete_bsp_segment_and_children(BSP_Segment *segment);

void delete_bsp_segment(BSP_Segment *segment);

void generate_BSP_maze(Maze const *maze);

void bsp_split(BSP_Segment *source, Maze const *maze);

void bsp_split_horizontal(BSP_Segment *source, Maze const *maze);

void bsp_split_vertical(BSP_Segment *source, Maze const *maze);

void bsp_split_recursive(BSP_Segment *parent, Maze const *maze);



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

    link_all_adjacent_cells(maze);

    // We will be splitting the grid and essentially connecting the 2 halves
    // each time until we finish

    BSP_Segment *starting_segment = new_bsp_segment(0, 0, width, height);
    bsp_split_recursive(starting_segment, maze);
}

BSP_Segment *new_bsp_segment(int start_x, int start_y, int end_x, int end_y) {
    BSP_Segment *segment = malloc(sizeof(BSP_Segment));
    segment->start_x = start_x;
    segment->start_y = start_y;
    segment->end_x = end_x;
    segment->end_y = end_y;
    segment->parent = NULL;
    segment->child1 = NULL;
    segment->child2 = NULL;
    return segment;
}

void delete_bsp_segment_and_children(BSP_Segment *segment) {
    if (segment == NULL) return;
    delete_bsp_segment_and_children(segment->child1);
    delete_bsp_segment_and_children(segment->child2);
    BSP_Segment *parent = segment->parent;
    if (parent != NULL) {
        if (parent->child1 == segment) {
            parent->child1 = NULL;
        }
        if (parent->child2 == segment) {
            parent->child2 = NULL;
        }
        segment->parent = NULL;
    }
    free(segment);
}

void delete_bsp_segment(BSP_Segment *segment) {
    if (segment == NULL) return;
    BSP_Segment *parent = segment->parent;
    BSP_Segment *child1 = segment->child1;
    BSP_Segment *child2 = segment->child2;

    if (parent != NULL) {
        if (parent->child1 == segment) {
            parent->child1 = NULL;
        }
        if (parent->child2 == segment) {
            parent->child2 = NULL;
        }
        segment->parent = NULL;
    }
    if (child1 != NULL) {
        segment->child1 = NULL;
        child1->parent = NULL;
    }
    if (child2 != NULL) {
        segment->child2 = NULL;
        child2->parent = NULL;
    }
    free(segment);
}

void bsp_split(BSP_Segment *source, Maze const *maze) {
    if (source == NULL || maze == NULL) return;
    if (source->child1 != NULL || source->child2 != NULL) {
        fprintf(stderr, "Cannot split an already split segment");
        return;
    }

    int range_x = source->end_x - source->start_x;
    int range_y = source->end_y - source->start_y;

    // find a pivot within the segment

    // make sure this isn't a single cell
    if (range_x != 0 || range_y != 0) {
        if (range_x == 0) {
            // cannot split vertically anymore
            bsp_split_horizontal(source, maze);
        } else if (range_y == 0) {
            // cannot split horizontally anymore
            bsp_split_vertical(source, maze);
        } else if(range_y > range_x) {
            bsp_split_horizontal(source, maze);
        } else if(range_x > range_y) {
            bsp_split_vertical(source, maze);
        } else {
            // randomly choose dimension to split in
            if (coin_flip()) {
                bsp_split_vertical(source, maze);
            } else {
                bsp_split_horizontal(source, maze);
            }
        }

    }

}

void bsp_split_horizontal(BSP_Segment *source, Maze const *maze) {
    int range_y = source->end_y - source->start_y;
    int range_x = source->end_x - source->start_x;
    int pivot_y;
    int passage_x;
    if (range_y == 0) {
        pivot_y = range_y + source->start_y;
    } else {
        pivot_y = (rand() % range_y) + source->start_y;
    }
    if (range_x == 0) {
        passage_x = range_x + source->start_x;
    } else {
        passage_x = (rand() % range_x) + source->start_x;
    }
    // ##########
    // #        #
    // x        #
    // #        #
    // ##########

    // Add a wall
    // ##########
    // #        #
    // #--------#
    // #        #
    // ##########

    // Add a gap to the wall
    // ##########
    // #        #
    // #---.----#
    // #        #
    // ##########

    for (int x = source->start_x; x < source->end_x; x++) {
        Cell *cell = cell_at(maze, x, pivot_y);
        if (cell == NULL) continue;
        if (x != passage_x) {
            unlink_cell_in_dir(cell, SOUTH);
        } else {
            //link_cell_in_dir(maze, cell, SOUTH);
        }
    }

    // return the 2 new sub-segments
    source->child1 = new_bsp_segment(
            source->start_x,
            source->start_y,
            source->end_x,
            pivot_y
    );
    source->child1->parent = source;

    source->child2 = new_bsp_segment(
            source->start_x,
            pivot_y+1,
            source->end_x,
            source->end_y
    );
    source->child2->parent = source;

}

void bsp_split_vertical(BSP_Segment *source, Maze const *maze) {
    int range_y = source->end_y - source->start_y;
    int range_x = source->end_x - source->start_x;
    int pivot_x;
    int passage_y;
    if (range_x == 0) {
        pivot_x = range_x + source->start_x;
    } else {
        pivot_x = (rand() % range_x) + source->start_x;
    }
    if (range_y == 0) {
        passage_y = range_y + source->start_y;
    } else {
        passage_y = (rand() % range_y) + source->start_y;
    }

    // #####x####
    // #        #
    // #        #
    // #        #
    // ##########

    // Add a wall
    // ##########
    // #    |   #
    // #    |   #
    // #    |   #
    // ##########

    // Add a gap to the wall
    // ##########
    // #    |   #
    // #    .   #
    // #    |   #
    // ##########

    for (int y = source->start_y; y < source->end_y; y++) {
        Cell *cell = cell_at(maze, pivot_x, y);
        if (cell == NULL) continue;
        if (y != passage_y) {
            unlink_cell_in_dir(cell, EAST);
        } else {
            //link_cell_in_dir(maze, cell, EAST);
        }
    }

    // return the 2 new sub-segments
    source->child1 = new_bsp_segment(
            source->start_x,
            source->start_y,
            pivot_x,
            source->end_y
    );
    source->child1->parent = source;

    source->child2 = new_bsp_segment(
            pivot_x+1,
            source->start_y,
            source->end_x,
            source->end_y
    );
    source->child2->parent = source;
}

void bsp_split_recursive(
        BSP_Segment *parent,
        Maze const *maze
) {
    if (parent == NULL || maze == NULL) return;

    bsp_split(parent, maze);
    BSP_Segment *child1 = parent->child1;
    BSP_Segment *child2 = parent->child2;

    // delete as we go to reduce memory usage
    delete_bsp_segment(parent);

    bsp_split_recursive(child1, maze);
    bsp_split_recursive(child2, maze);
}

#endif //MAZE_BSP_H
