#ifndef MAZE_KRUSKAL_H
#define MAZE_KRUSKAL_H

#include "../Maze.h"
#include "../utils.h"

void generate_kruskal_maze(const Maze *maze) {
    if (maze == NULL) {
        fprintf(stderr, "No maze given to generator");
        exit(EXIT_FAILURE);
    }
    const int width = maze->width;
    const int height = maze->height;
    const int total_cells = maze->cell_count;

    unlink_all_cells(maze);

    // Create an initial set of nodes that are all not connected
    CellTreeNode *nodes[height][width];
    CellTreeNode *first_non_null = NULL;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            if (cell == NULL) {
                nodes[y][x] = NULL;
                continue;
            }
            CellTreeNode *node = new_cell_tree_node(cell);
            nodes[y][x] = node;
            if (first_non_null == NULL) first_non_null = node;
        }
    }

    CellTreeNode *root = first_non_null;
    int total_connected_count;
    do {
        // get a cell and an unlinked neighbour that aren't in the same
        Cell *cell = NULL;
        Cell *unlinked = NULL;
        CellTreeNode *node1 = NULL;
        CellTreeNode *node2 = NULL;

        while (cell == NULL && unlinked == NULL) {
            cell = random_cell(maze);
            unlinked = random_unlinked_cell((Maze *) maze, cell);
            if (unlinked == NULL) continue;
            node1 = nodes[cell->y][cell->x];
            node2 = nodes[unlinked->y][unlinked->x];

            bool in_same_tree = in_same_cell_tree(node1, node2);
            if (!in_same_tree) {
                link_adjacent_cells(cell, unlinked);
                append_cell_tree_node(node1, node2);
            } else {
                cell = NULL;
                unlinked = NULL;
            }
        }

        root = get_root_cell_tree_node(root);
        total_connected_count = total_cells_in_tree(root);
    } while (total_connected_count < total_cells);

    root = get_root_cell_tree_node(root);
    delete_cell_tree(root);
}

#endif //MAZE_KRUSKAL_H
