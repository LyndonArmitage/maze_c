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
    return rand() % (WEST + 1);
}

Cell *random_cell(Maze *maze) {
    if (maze == NULL) return NULL;
    Cell *cell = NULL;
    while (cell == NULL) {
        int x = rand() % maze->width;
        int y = rand() % maze->height;
        cell = cell_at(maze, x, y);
    }
    return cell;
}

typedef struct CellListEntry {
    Cell *cell;
    struct CellListEntry *previous;
    struct CellListEntry *next;
} CellListEntry;

CellListEntry *new_cell_list_entry(Cell *start) {
    if (start == NULL) return NULL;
    CellListEntry *list = malloc(sizeof(CellListEntry));
    list->cell = start;
    list->previous = NULL;
    list->next = NULL;
    return list;
}

CellListEntry *peek_last_cell_list(CellListEntry *start) {
    if (start == NULL) return NULL;
    CellListEntry *current = start;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

CellListEntry *peek_first_cell_list(CellListEntry *entry) {
    if (entry == NULL) return NULL;
    CellListEntry *current = entry;
    while (current->previous != NULL) {
        current = current->previous;
    }
    return current;
}

void push_to_cell_list(CellListEntry *start, Cell *cell) {
    if (start == NULL || cell == NULL) return;
    CellListEntry *last = peek_last_cell_list(start);
    CellListEntry *next = new_cell_list_entry(cell);
    next->previous = last;
    last->next = next;
}

CellListEntry *delete_from_cell_list(CellListEntry *entry) {
    if (entry == NULL) return NULL;

    // Both of these can be NULL
    CellListEntry *previous = entry->previous;
    CellListEntry *next = entry->next;

    // return NULL if the whole list is gone
    if (next == NULL && previous == NULL) {
        free(entry);
        return NULL;
    }

    if (previous != NULL) {
        previous->next = next;
    }
    if (next != NULL) {
        next->previous = previous;
    }
    free(entry);

    // If this was the first entry in a list then return the next,
    // otherwise return the previous
    if (previous == NULL) {
        return next;
    } else {
        return previous;
    }
}

Cell *pop_from_cell_list(CellListEntry *list) {
    if (list == NULL) return NULL;
    CellListEntry *last = peek_last_cell_list(list);
    Cell *cell = last->cell;
    delete_from_cell_list(last);
    return cell;
}

void delete_all_after(CellListEntry *entry) {
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
