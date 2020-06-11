#include <stdio.h>
#include "Maze.h"
#include "generator/BinaryTree.h"

int main() {
    Maze *maze = new_maze(5, 5, false);

    generate_binary_tree_maze(maze);

    print_maze(maze);
    delete_maze(maze);
    return 0;
}
