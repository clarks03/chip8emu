#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include "chip8.h"
#include "graphics.h"

#define SCALE 10
#define WIDTH 64
#define HEIGHT 32

SDL_Window *window;
SDL_Renderer *renderer;

/* int key_map[16] = { */
/*     SDLK_x, SDLK_1, SDLK_2, SDLK_3, */
/*     SDLK_q, SDLK_w, SDLK_e, SDLK_a, */
/*     SDLK_s, SDLK_d, SDLK_z, SDLK_c, */
/*     SDLK_4, SDLK_r, SDLK_f, SDLK_v */
/* }; */
int key_map[16] = {
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

void setup_graphics() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create a window
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error :%s\n", SDL_GetError());
        exit(1);
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error :%s\n", SDL_GetError());
        exit(1);
    }
}

void draw_graphics(chip8 *chip) {
    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = { 0, 0, SCALE, SCALE };

    // Draw the pixels
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (chip->gfx[y * WIDTH + x] == 1) {
                rect.x = x * SCALE;
                rect.y = y * SCALE;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Present the renderer
    SDL_RenderPresent(renderer);
}

void set_keys(chip8 *chip) {

    // Reset key state
    for (char i = 0; i < 16; i++) 
        chip->key[i] = 0;

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            free(chip);
            SDL_Quit();
            exit(0);
        }
    }


    // Update key states based on key map
    for (char i = 0; i < 16; i++) {
        if (state[key_map[i]]) {
            chip->key[i] = 1;  // Key is pressed
        }
    }

}
