#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>

typedef struct {
    // The chip 8 has 35 opcodes, each being 2 bytes long
    unsigned short opcode;

    // The chip 8 has 4K memory in totial
    unsigned char memory[4096];

    // The Chip 8 has 15 8-bit general-purpose registers named
    // V0, V1, ..., VE (hex)
    unsigned char V[16];

    // There is an index register I and a program counter (pc)
    // which can have a value from 0x000-0xFFF
    unsigned short I;
    unsigned short pc;

    // Memory map:
    // 0x000-0x1FF <- chip 8 interpreter (contains font set in emu)
    // 0x050-0x0A0 <- used for the built-in 4x5 pixel font set (0-F)
    // 0x200-0xFFF <- program ROM and work RAM

    // Graphics
    // The graphics are b/w and have 2048 pixels (64x32)
    unsigned char gfx[64 * 32];  // 1 or 0

    // There are two timer registers that count at 60hz
    unsigned char delay_timer;
    unsigned char sound_timer;

    // The Chip 8 allows for jumping to a certain address, so you will need to store
    // previous address in a stack. The system has 16 levels of stack
    // and so you need a stack and sp (stack pointer)
    unsigned short stack[16];
    unsigned short sp;

    unsigned char draw_flag;

    // Hex-based keypad so you can use an array to store the current state of the key
    unsigned char key[16];
} chip8;


// Chip 8 font set
extern unsigned char chip8_fontset[80];

void my_chip8_initialize(chip8 *chip);
void my_chip8_load_game(chip8 *chip, FILE *game);
void my_chip8_emulate_cycle(chip8 *chip);

#endif
