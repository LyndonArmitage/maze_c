#ifndef MAZE_SDL_MAZE_RENDERER_H
#define MAZE_SDL_MAZE_RENDERER_H

#include <SDL2/SDL.h>
#include "Maze.h"

void render_maze_to_sdl(SDL_Renderer *renderer, const Maze *maze, int cell_size);

int render_maze_with_refresh(
        const Maze *maze,
        int cell_size,
        void (*maze_generator)(const Maze *)
) {
    if (maze == NULL || cell_size < 1 || maze_generator == NULL) {
        fprintf(stderr, "Invalid arguments for rendering");
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to init video: %s\n", SDL_GetError());
        return 1;
    }

    const int window_width = maze->width * cell_size;
    const int window_height = maze->height * cell_size;

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

    maze_generator(maze);

    render_maze_to_sdl(renderer, maze, cell_size);
    bool done = false;
    SDL_Event event;
    while (!done && SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_MOVED:
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_RESTORED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        render_maze_to_sdl(renderer, maze, cell_size);
                        break;
                    default:
                        break;
                }
            }
                break;
            case SDL_KEYUP: {
                SDL_KeyCode code = event.key.keysym.sym;
                if (code == SDLK_ESCAPE) {
                    done = true;
                } else {
                    maze_generator(maze);
                    render_maze_to_sdl(renderer, maze, cell_size);
                }
                break;
            }
        }
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}

void render_maze_to_sdl(SDL_Renderer *renderer, const Maze *maze, int cell_size) {
    if (renderer == NULL || maze == NULL) {
        return;
    }

    const int width = maze->width;
    const int height = maze->height;

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
