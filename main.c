#include <stdio.h>
#include "Maze.h"
#include "generator/BinaryTree.h"
#include "SDL_Maze_Renderer.h"

int main() {
    Maze *maze = new_maze(5, 5, false);

    generate_binary_tree_maze(maze);
    render_maze(maze, 50);
    delete_maze(maze);
    return 0;
}
