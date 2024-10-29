#include "chip8.h"  // CPU core implementation
#include "graphics.h"
#include <stdlib.h>
#include <stdio.h>

chip8 *my_chip8;

#define FPS 120

// Emulation loop
const Uint32 frame_duration = 1000 / FPS;  // Duration of each frame in milliseconds

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("Usage: ./chip8_emulator <ROM path>");
        exit(1);
    }
    // Setup render system and register input callbacks
    setup_graphics();
    // setup_input();
    
    // Initialize the Chip8 system and load the game into the memory
    my_chip8 = malloc(sizeof(chip8));
    my_chip8_initialize(my_chip8);
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Cannot open file\n");
        exit(1);
    }
    my_chip8_load_game(my_chip8, fp);

    // Emulation loop
    int frames = 0;
    for (;;) {
        Uint32 start_time = SDL_GetTicks();

        // Emulate one cycle
        my_chip8_emulate_cycle(my_chip8);

        // If the draw flag is set, update the screen
        if (my_chip8->draw_flag) {
            draw_graphics(my_chip8);
            my_chip8->draw_flag = 0;
        }

        // Store key press state (press and release)
        set_keys(my_chip8);

        // Calculate how long the last frame took
        Uint32 frame_time = SDL_GetTicks() - start_time;

        // If the frame took less time than the frame duration, delay
        if (frame_time < frame_duration)
            SDL_Delay(frame_duration - frame_time);  // Delay to maintain 60FPS
        
        /* printf("Frame %d\n", frames); */
        /* frames++; */
    }

    return 0;
}
