#ifndef MAZE_RANDOM_H
#define MAZE_RANDOM_H

#include <stdbool.h>
#include <stdlib.h>

#define HALF_RAND_MAX (RAND_MAX /2)

bool coin_flip() {
    return rand() <= HALF_RAND_MAX;
}

#endif //MAZE_RANDOM_H
