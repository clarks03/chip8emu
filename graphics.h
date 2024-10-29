#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include "chip8.h"

void setup_graphics();
void draw_graphics(chip8 *chip);
void set_keys(chip8 *chip);

extern int key_map[16];

#endif
