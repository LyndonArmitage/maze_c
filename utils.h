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

Cell *random_cell(const Maze*maze) {
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

Cell** cell_list_to_array(CellListEntry const *start, int *size) {
    int count = length_of_cell_list(start);
    Cell **array = NULL;
    if (count <= 0) {
        *size = 0;
        return array;
    }
    array = malloc(sizeof(Cell *) * count);
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

#endif //MAZE_UTILS_H
