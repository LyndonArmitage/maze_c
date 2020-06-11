#ifndef MAZE_SDL_MAZE_RENDERER_H
#define MAZE_SDL_MAZE_RENDERER_H

#include <SDL2/SDL.h>
#include "Maze.h"

void render_maze_to_sdl(SDL_Renderer *renderer, Maze *maze, int cell_size);


int render_maze(Maze *maze, int cell_size) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to init video: %s\n", SDL_GetError());
        return 1;
    }

    int window_width = maze->width * cell_size;
    int window_height = maze->height * cell_size;

    SDL_Window *window = SDL_CreateWindow(
            "Maze",
            0, 0,
            window_width, window_height,
            SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_SOFTWARE
    );
    if (renderer == NULL) {
        fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    bool done = false;
    while (!done) {
        render_maze_to_sdl(renderer, maze, cell_size);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
            }
        }
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}

void render_maze_to_sdl(SDL_Renderer *renderer, Maze *maze, int cell_size) {
    if (renderer == NULL || maze == NULL) {
        return;
    }

    int width = maze->width;
    int height = maze->height;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell *cell = cell_at(maze, x, y);
            if (cell == NULL) continue;
            Directions dirs = get_blocked_directions(cell);
            if (dirs.north) {
                SDL_RenderDrawLine(
                        renderer,
                        x * cell_size, y * cell_size,
                        (x * cell_size) + cell_size, y * cell_size
                );
            }
            if (dirs.south) {
                SDL_RenderDrawLine(
                        renderer,
                        x * cell_size, (y * cell_size) + cell_size,
                        (x * cell_size) + cell_size, (y * cell_size) + cell_size
                );
            }
            if (dirs.west) {
                SDL_RenderDrawLine(
                        renderer,
                        x * cell_size, y * cell_size,
                        x * cell_size, (y * cell_size) + cell_size
                );
            }
            if (dirs.east) {
                SDL_RenderDrawLine(
                        renderer,
                        (x * cell_size) + cell_size, y * cell_size,
                        (x * cell_size) + cell_size, (y * cell_size) + cell_size
                );
            }
        }
    }
    SDL_RenderPresent(renderer);
}

#endif //MAZE_SDL_MAZE_RENDERER_H