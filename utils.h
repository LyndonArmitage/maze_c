#ifndef MAZE_UTILS_H
#define MAZE_UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include "Maze.h"

#define HALF_RAND_MAX (RAND_MAX /2)

bool coin_flip() {
    return rand() <= HALF_RAND_MAX;
}

int random_direction() {
    return rand() % DIRECTION_COUNT;
}

Cell *random_linked_cell(Cell *cell) {
    if (cell == NULL || cell->neighbour_count <= 0) return NULL;
    int neighbour_count = cell->neighbour_count;

    // Check to ensure there are valid neighbours
    int null_count = 0;
    for (int i = 0; i < neighbour_count; i++) {
        if (cell->neighbours[i] == NULL) null_count++;
        else break;
    }
    if (null_count >= neighbour_count) return NULL;

    int pos;
    Cell *linked = NULL;
    while (linked == NULL) {
        pos = rand() % neighbour_count;
        linked = cell->neighbours[pos];
    }
    return linked;
}

Cell *random_unlinked_cell(Maze *maze, Cell *cell) {
    if (cell == NULL) return NULL;

    // create an array of all the possible neighbours
    int neighbour_count = 0;
    Cell **possible = get_all_neighbouring_cells(maze, cell, &neighbour_count);

    // Ensure this cell has neighbours
    int null_count = 0;
    for (int i = 0; i < neighbour_count; i++) {
        Cell *other = possible[i];
        if (other == NULL) {
            null_count++;
        } else {
            // NULL out any connected
            int dir = neighbour_pos(cell, other);
            if (dir != -1 && cell->neighbours[dir] == other) {
                possible[i] = NULL;
                null_count++;
            }
        }
    }
    if (null_count >= neighbour_count) {
        free(possible);
        return NULL;
    }

    Cell *unlinked = NULL;
    while (unlinked == NULL) {
        int dir = rand() % neighbour_count;
        unlinked = possible[dir];
    }

    free(possible);
    return unlinked;
}

Cell *random_neighbour_cell(Maze *maze, Cell *cell) {
    if (maze == NULL || cell == NULL) return NULL;

    // create an array of all the possible neighbours
    int neighbour_count = 0;
    Cell **possible = get_all_neighbouring_cells(maze, cell, &neighbour_count);

    // Ensure this cell has neighbours
    int null_count = 0;
    for (int i = 0; i < neighbour_count; i++) {
        if (possible[i] == NULL) null_count++;
        else break;
    }
    if (null_count >= neighbour_count) {
        free(possible);
        return NULL;
    }

    int dir;
    Cell *neighbour = NULL;
    while (neighbour == NULL) {
        dir = rand() % neighbour_count;
        neighbour = possible[dir];
    }
    free(possible);
    return neighbour;
}

Cell *random_cell(const Maze *maze) {
    if (maze == NULL) return NULL;
    Cell *cell = NULL;
    while (cell == NULL) {
        int x = rand() % maze->width;
        int y = rand() % maze->height;
        cell = cell_at(maze, x, y);
    }
    return cell;
}

/**
 * A simple doubly linked list type for Cells
 */
typedef struct CellListEntry {
    Cell *cell;
    struct CellListEntry *previous;
    struct CellListEntry *next;
} CellListEntry;

/**
 * Create a new Cell linked list using the given start cell.
 *
 * @param start The starting Cell, can be NULL
 * @return A pointer to a new CellListEntry
 */
CellListEntry *new_cell_list_entry(const Cell *start) {
    CellListEntry *list = malloc(sizeof(CellListEntry));
    if (list == NULL) {
        fprintf(stderr, "Unable to allocate cell list entry");
        exit(EXIT_FAILURE);
    }
    list->cell = (Cell *) start;
    list->previous = NULL;
    list->next = NULL;
    return list;
}

/**
 * Get the last CellListEntry in a given list.
 *
 * @param start The CellListEntry to start from
 * @return A pointer to the last CellListEntry in a given list.
 * Can be the same as the starting entry or NULL if the start was NULL.
 */
CellListEntry *peek_last_cell_list(const CellListEntry *start) {
    if (start == NULL) return NULL;
    CellListEntry *current = (CellListEntry *) start;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

/**
 * Get the first CellListEntry in a given list
 *
 * @param entry The entry to start searching from
 * @return A pointer to the first CellListEntry in a given list.
 * Can be the same as the entry given, or NULL if entry was NULL.
 */
CellListEntry *peek_first_cell_list(CellListEntry *entry) {
    if (entry == NULL) return NULL;
    CellListEntry *current = entry;
    while (current->previous != NULL) {
        current = current->previous;
    }
    return current;
}

/**
 * Push a given cell to the linked list.
 *
 * @param start The start of the linked list
 * @param cell The cell to add to the end
 * @return The new CellListEntry or NULL if start was NULL
 */
CellListEntry *push_to_cell_list(CellListEntry *start, Cell *cell) {
    if (start == NULL) return NULL;
    CellListEntry *last = peek_last_cell_list(start);
    CellListEntry *next = new_cell_list_entry(cell);
    next->previous = last;
    last->next = next;
    return next;
}

enum CellListDeleteResult {
    ENTRY_DELETED = 0,
    LIST_DELETED = 1
};

/**
 * Removes and deletes a CellListEntry from the list it is part of
 *
 * @param entry The entry to delete
 * @return 0 if the entry was deleted, 1 if the whole list was deleted
 */
int delete_from_cell_list(CellListEntry *entry) {
    if (entry == NULL) return LIST_DELETED;

    // Both of these can be NULL
    CellListEntry *previous = entry->previous;
    CellListEntry *next = entry->next;

    // return NULL if the whole list is gone
    if (next == NULL && previous == NULL) {
        free(entry);
        entry = NULL;
        return LIST_DELETED;
    }

    // Connect the other members of the list so connections are not lost
    if (previous != NULL) {
        previous->next = next;
    }
    if (next != NULL) {
        next->previous = previous;
    }
    free(entry);
    entry = NULL;

    return ENTRY_DELETED;
}

/**
 * Deletes the last added CellListEntry from a given list and returns the Cell
 * it referenced.
 * @param list The linked list to pop from
 * @return The last Cell referenced in the list whose entry has now been deleted.
 * Can be NULL if the list was NULL or the CellListEntry was pointing at a NULL
 * Cell.
 */
Cell *pop_last_from_cell_list(CellListEntry *list) {
    if (list == NULL) return NULL;
    CellListEntry *last = peek_last_cell_list(list);
    Cell *cell = last->cell;
    delete_from_cell_list(last);
    return cell;
}

/**
 * Deleted all cells after entry in the list that entry is a part of.
 * @param entry The CellListEntry to use as a point of reference and delete
 * after.
 */
void delete_all_after(CellListEntry const *entry) {
    if (entry == NULL) return;
    CellListEntry *last = peek_last_cell_list(entry);
    if (last == entry) return;

    CellListEntry *current = last;
    CellListEntry *previous = NULL;
    do {
        previous = current->previous;
        delete_from_cell_list(current);
        current = previous;
    } while (current != NULL && current != entry);
}

/**
 * Delete an entire Cell linked list
 * @param start The start of the linked list
 */
void delete_cell_list(CellListEntry *start) {
    CellListEntry *current = start;
    while (current != NULL) {
        CellListEntry *to_delete = current;
        current = current->next;
        free(to_delete);
        to_delete = NULL;
    }
}

/**
 * Get the length of a Cell linked list
 *
 * @param start The starting entry
 * @return The length of the linked list
 */
int length_of_cell_list(CellListEntry const *start) {
    if (start == NULL) return 0;
    int count = 1;
    CellListEntry *current = (CellListEntry *) start;
    while (current->next != NULL) {
        current = current->next;
        count++;
    }
    return count;
}

/**
 * Pick a random Cell from the given Cell linked list
 * @param start The start of the linked list
 * @return A random cell from the linked list.
 * This can be NULL if start was NULL or the linked list contains NULL entries
 */
Cell *pick_from_cell_list(CellListEntry const *start) {
    if (start == NULL) return NULL;
    int count = length_of_cell_list(start);
    int pos_to_return = rand() % count;
    int pos = 0;
    CellListEntry *current = (CellListEntry *) start;
    while (pos < pos_to_return && current->next != NULL) {
        current = current->next;
        pos++;
    }
    return current->cell;
}

Cell **cell_list_to_array(CellListEntry const *start, int *size) {
    int count = length_of_cell_list(start);
    Cell **array = NULL;
    if (count <= 0) {
        *size = 0;
        return array;
    }
    array = malloc(sizeof(Cell *) * count);
    if (array == NULL) {
        fprintf(stderr, "Unable to allocate array for cell list");
        exit(EXIT_FAILURE);
    }
    CellListEntry *current = (CellListEntry *) start;
    array[0] = start->cell;
    int pos = 1;
    while (current->next != NULL) {
        current = current->next;
        array[pos] = current->cell;
        pos++;
    }
    *size = count;
    return array;
}

/**
 * Tree like data structure of cells, can be used as a set
 */
typedef struct CellTreeNode {
    Cell *cell;
    struct CellTreeNode *parent;
    struct CellTreeNode **children;
    int child_count;
} CellTreeNode;

CellTreeNode *new_cell_tree_node(Cell *cell);

void delete_cell_tree(CellTreeNode *root);

CellTreeNode *add_child_cell_tree_node(CellTreeNode *parent, Cell *cell);

void append_cell_tree_node(CellTreeNode *parent, CellTreeNode *child);

CellTreeNode *get_root_cell_tree_node(CellTreeNode *node);

bool in_same_cell_tree(CellTreeNode *node1, CellTreeNode *node2);

void combine_cell_trees(CellTreeNode *tree1, CellTreeNode *tree2);

bool in_cell_tree(const CellTreeNode *root, const Cell *cell);

CellTreeNode *new_cell_tree_node(Cell *cell) {
    if (cell == NULL) return NULL;
    CellTreeNode *node = malloc(sizeof(CellTreeNode));
    if (node == NULL) {
        fprintf(stderr, "Unable to allocated CellTreeNode");
        exit(EXIT_FAILURE);
    }
    node->cell = cell;
    node->parent = NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

void delete_cell_tree(CellTreeNode *root) {
    if (root == NULL) return;
    CellTreeNode **children = root->children;
    if (children != NULL) {
        for (int i = 0; i < root->child_count; i++) {
            CellTreeNode *child = children[i];
            delete_cell_tree(child);
        }
        root->children = NULL;
        root->child_count = 0;
        free(children); // free up the child count
    }

    if (root->parent != NULL) {
        // TODO: Improve removing reference from parent
        // Could make a function that will rearrange and allocate the children
        // array.

        // Remove reference from parent
        for (int i = 0; i < root->child_count; i++) {
            CellTreeNode *child = root->children[i];
            if (child == root) {
                // This leaves a NULL in the parent and no change to child_count
                root->children[i] = NULL;
                break;
            }
        }
        root->parent = NULL;
    }

    root->cell = NULL;
    free(root);
    root = NULL;
}

CellTreeNode *add_child_cell_tree_node(CellTreeNode *parent, Cell *cell) {
    if (parent == NULL || cell == NULL) return NULL;
    CellTreeNode *node = new_cell_tree_node(cell);
    append_cell_tree_node(parent, node);
    return node;
}

void append_cell_tree_node(CellTreeNode *parent, CellTreeNode *child) {
    if (parent == NULL || child == NULL) return;
    CellTreeNode *child_root = get_root_cell_tree_node(child);
    if (in_same_cell_tree(parent, child_root)) return; // already in same tree

    int count = parent->child_count + 1;
    CellTreeNode **pointer;
    if (parent->children == NULL) {
        pointer = malloc(sizeof(CellTreeNode *) * count);
    } else {
        pointer = realloc(parent->children, sizeof(CellTreeNode *) * count);
    }
    if (pointer == NULL) {
        fprintf(stderr, "Unable to grow cell tree to size %d", count);
        exit(EXIT_FAILURE);
    }

    parent->children = pointer;
    parent->child_count = count;
    pointer[count - 1] = child_root;
    child_root->parent = parent;

    return;
}

CellTreeNode *get_root_cell_tree_node(CellTreeNode *node) {
    if (node == NULL) return NULL;
    CellTreeNode *root = node;
    while (root->parent != NULL) {
        root = root->parent;
    }
    return root;
}

bool in_same_cell_tree(CellTreeNode *node1, CellTreeNode *node2) {
    if (node1 == NULL || node2 == NULL) return false;
    CellTreeNode *root1 = get_root_cell_tree_node(node1);
    CellTreeNode *root2 = get_root_cell_tree_node(node2);
    return root1 == root2;
}

void combine_cell_trees(CellTreeNode *tree1, CellTreeNode *tree2) {
    if (tree1 == NULL || tree2 == NULL) return;
    CellTreeNode *root1 = get_root_cell_tree_node(tree1);
    CellTreeNode *root2 = get_root_cell_tree_node(tree2);
    if (in_same_cell_tree(root1, root2)) return;
    append_cell_tree_node(root1, root2);
}

bool in_cell_tree(const CellTreeNode *root, const Cell *cell) {
    if (root == NULL || cell == NULL) return false;
    if (root->cell == cell) return true;
    int count = root->child_count;
    for (int i = 0; i < count; i++) {
        CellTreeNode *child = root->children[i];
        if (child != NULL) {
            // recursive check
            if (in_cell_tree(child, cell)) {
                return true;
            }
        }
    }
    return false;
}

int total_cells_in_tree(const CellTreeNode *root) {
    if (root == NULL) return 0;
    int child_count = root->child_count;
    if (child_count <= 0) return 1;

    int count = 1;
    for (int i = 0; i < child_count; i++) {
        int inc = total_cells_in_tree(root->children[i]);
        count += inc;
    }
    return count;
}


#endif //MAZE_UTILS_H
