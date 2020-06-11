#ifndef MAZE_UTILS_H
#define MAZE_UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include "Maze.h"

#define HALF_RAND_MAX (RAND_MAX /2)

bool coin_flip() {
    return rand() <= HALF_RAND_MAX;
}

typedef struct CellListEntry {
    Cell *cell;
    struct CellListEntry *previous;
    struct CellListEntry *next;
} CellListEntry;

CellListEntry *new_cell_list_entry(Cell *start) {
    CellListEntry *list = malloc(sizeof(CellListEntry));
    list->cell = start;
    list->previous = NULL;
    list->next = NULL;
    return list;
}

void push_to_cell_list(CellListEntry *start, Cell *cell) {
    CellListEntry *current = start;
    while (current->next != NULL) {
        current = current->next;
    }
    CellListEntry *next = new_cell_list_entry(cell);
    next->previous = current;
    current->next = next;
}

void delete_cell_list(CellListEntry *start) {
    CellListEntry *current = start;
    while (current != NULL) {
        CellListEntry *to_delete = current;
        current = current->next;
        free(to_delete);
    }
}

Cell *pick_from_cell_list(CellListEntry *start) {
    if (start == NULL) return NULL;
    int count = 1;
    CellListEntry *current = start;
    while (current->next != NULL) {
        current = current->next;
        count++;
    }
    int pos_to_return = rand() % count;
    int pos = 0;
    current = start;
    while (pos < pos_to_return && current->next != NULL) {
        current = current->next;
        pos++;
    }
    return current->cell;
}

#endif //MAZE_UTILS_H
