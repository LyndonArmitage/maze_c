#include <stdio.h>
#include <time.h>
#include "Maze.h"
#include "generator/BinaryTree.h"
#include "generator/Sidewinder.h"
#include "SDL_Maze_Renderer.h"

int main() {
    srand(time(NULL));
    Maze *maze = new_maze(10, 10, false);

    //generate_binary_tree_maze(maze);
    generate_sidewinder_maze(maze);
    render_maze(maze, 50);
    delete_maze(maze);
    return 0;
}
