#include <stdio.h>
#include <time.h>
#include "Maze.h"
#include "generator/BinaryTree.h"
#include "generator/Sidewinder.h"
#include "generator/Aldous_Broder.h"
#include "generator/HuntKill.h"
#include "SDL_Maze_Renderer.h"

int main() {
    srand(time(NULL));
    Maze *maze = new_maze(19, 10, false);

    render_maze_with_refresh(maze, 50, generate_hunt_and_kill_maze);
    delete_maze(maze);
    return 0;
}
