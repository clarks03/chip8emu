#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

unsigned char chip8_fontset[80] = { 0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

void my_chip8_initialize(chip8 *chip) {
    // Initialize registers and memory once
    chip->pc     = 0x200;  // Program counter starts at 0x200
    chip->opcode = 0;      // Reset current opcode
    chip->I      = 0;      // Reset index register
    chip->sp     = 0;      // Reset stack pointer

    for (short i = 0; i < 4096; i++) {
        if (i < 16) {
            // Clear stack
            chip->stack[i] = 0;
            // Clear registers V0-VF
            chip->V[i] = 0;
        }
        if (i < 2048)
        // Clear display
            chip->gfx[i] = 0;

        // Clear memory
        chip->memory[i] = 0;
    }

    // Load fontset
    for (int i = 0; i < 80; ++i)
        chip->memory[i] = chip8_fontset[i];

    // Reset timers
    chip->sound_timer = 60;  // Is this what it should be?
    chip->delay_timer = 60;  // Is this what it should be?
    
    // Setting draw flag
    chip->draw_flag = 0;
}

void my_chip8_emulate_cycle(chip8 *chip) {
    // Fetch Opcode
    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
    /* printf("Opcode: 0x%x\n", chip->opcode); */
    /* printf("PC: 0x%x\n", chip->pc); */
    /* printf("SP: 0x%x\n", chip->sp); */

    // Decode & execute Opcode
    switch (chip->opcode & 0xF000) {

        case 0x0000:
            switch (chip->opcode & 0x000F) {
                case 0x0000:  // 0x00E0: Clears the screen
                    memset(chip->gfx, 0, 64 * 32);
                    chip->draw_flag = 1;
                break;

                case 0x000E:  // 0x00EE: Returns from subroutine
                    --chip->sp;
                    chip->pc = chip->stack[chip->sp];
                    chip->pc += 2;
                break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
                break;
            }
        break;

        case 0x1000:  // 1NNN: jumps to address NNN
            chip->pc = chip->opcode & 0x0FFF;
        break;

        case 0x2000:  // Calls subroutine at NNN
            chip->stack[chip->sp] = chip->pc;
            ++chip->sp;
            chip->pc = chip->opcode & 0x0FFF;
        break;

        case 0x3000:  // 0x3XNN: Skips the next instruction if VX == NN
            if (chip->V[(chip->opcode & 0x0F00) >> 8] == (chip->opcode & 0x00FF))
                chip->pc += 4;
            else
                chip->pc += 2;
        break;

        case 0x4000:  // 0x4XNN: Skips the next instruction if VX != NN
            if (chip->V[(chip->opcode & 0x0F00) >> 8] != (chip->opcode & 0x00FF))
                chip->pc += 4;
            else
                chip->pc += 2;
        break;

        case 0x5000:  // 0x5XY0: Skips the next instruction if VX == VY
            if (chip->V[(chip->opcode & 0x0F00) >> 8] == (chip->V[(chip->opcode & 0x00F0)] >> 4))
                chip->pc += 4;
            else
                chip->pc += 2;
        break;

        case 0x6000:  // 0x6XNN: Sets VX to NN
            chip->V[(chip->opcode & 0x0F00) >> 8] = (chip->opcode & 0x00FF);
            chip->pc += 2;
        break;

        case 0x7000:  // 0x7XNN: Adds NN to VX (carry flag not changed)
            chip->V[(chip->opcode & 0x0F00) >> 8] += (chip->opcode & 0x00FF);
            chip->pc += 2;
        break;

        case 0x8000:
            switch (chip->opcode & 0x000F) {
                case 0x0000:  // 0x8XY0: sets VX to the value of VY
                    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;

                case 0x0001:  // 0x8XY1: sets VX to VX or VY (bitwise)
                    chip->V[(chip->opcode & 0x0f00) >> 8] |= chip->V[(chip->opcode & 0x00f0) >> 4];
                    chip->pc += 2;
                break;

                case 0x0002:  // 0x8XY2: sets VX to VX and VY (bitwise)
                    chip->V[(chip->opcode & 0x0F00) >> 8] &= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;

                case 0x0003:  // 0x8XY3: sets VX to VX xor VY
                    chip->V[(chip->opcode & 0x0F00) >> 8] ^= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;

                case 0x0004:  // 0x8XY4: adds VY to VX. FF is set to 0 when there's an underflow, and 1 if there's not
                    if (chip->V[(chip->opcode & 0x00F0) >> 4] > (0xFF - chip->V[(chip->opcode & 0x0F00) >> 8]))
                        chip->V[0xF] = 1;  // carry
                    else
                        chip->V[0xF] = 0;
                    chip->V[(chip->opcode & 0x0F00) >> 8] += chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;

                case 0x0005:  // 0x8XY5: VY is subtracted from VX
                    if (chip->V[(chip->opcode & 0x0F00) >> 8] >= chip->V[(chip->opcode & 0x00F0) >> 4])
                        chip->V[0xF] = 1;
                    else
                        chip->V[0xF] = 0;
                    chip->V[(chip->opcode & 0x0F00) >> 8] -= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;

                case 0x0006:  // 0x8XY6: Shifts VX to the right by 1
                    chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] & 0x01;
                    chip->V[(chip->opcode & 0x0F00) >> 8] >>= 1;
                    chip->pc += 2;
                break;

                case 0x0007:  // 0x8XY7: Sets VX to VY minus VX
                    if (chip->V[(chip->opcode & 0x00F0) >> 4] >= chip->V[(chip->opcode & 0x0F00) >> 8])
                        chip->V[0xF] = 1;
                    else
                        chip->V[0xF] = 0;
                    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->V[(chip->opcode & 0x00F0) >> 4] - chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                break;

                case 0x000E:  // 0x8XYE: Shifts VS to the left by 1
                    chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] & 0x80;
                    chip->V[(chip->opcode & 0x0F00) >> 8] <<= 1;
                    chip->pc += 2;
                break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", chip->opcode);
            }
        break;

        case 0x9000:  // 0x9XY0: Skips the next instruction if VX != VY
            if (chip->V[(chip->opcode & 0x0F00) >> 8] != (chip->V[(chip->opcode & 0x00F0)] >> 4))
                chip->pc += 4;
            else
                chip->pc += 2;
        break;

        case 0xA000:  // 0xANNN: sets I to the address NNN
            chip->I = chip->opcode & 0x0FFF;
            chip->pc += 2;
        break;

        case 0xB000:  // 0xBNNN: Jumps to the address NNN + V0
            chip->pc = chip->opcode & 0xFFF + chip->V[0x0];
        break;

        case 0xC000:  // 0xCXNN: Sets VX to the result of a bitwise AND on a random number [0, 255] and NN
            srand(time(NULL));
            char random = rand() % 0xFF;
            chip->V[(chip->opcode & 0x0F00) >> 8] = random & (chip->opcode & 0x00FF);
            chip->pc += 2;
        break;

        case 0xD000:  // 0xDXYN: Drawing
            // Execute opcode
            unsigned short x = chip->V[(chip->opcode & 0x0F00) >> 8];
            unsigned short y = chip->V[(chip->opcode & 0x00F0) >> 4];
            unsigned short height = chip->opcode & 0x000F;
            unsigned short pixel;

            chip->V[0xF] = 0;
            for (int yline = 0; yline < height; yline++) {
                pixel = chip->memory[chip->I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (chip->gfx[(x + xline + ((y + yline) * 64))] == 1)
                            chip->V[0xF] = 1;
                        chip->gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            chip->draw_flag = 1;
            chip->pc += 2;
        break;

        case 0xE000:
            switch(chip->opcode & 0x000F) {
                case 0x0001:  // 0xEX9E: Skips the next instruction if the key stored in VX is not pressed
                    if (chip->key[chip->V[(chip->opcode & 0x0F00) >> 8]] != 1)
                        chip->pc += 4;
                    else
                        chip->pc += 2;
                break;

                case 0x000E:  // 0xEXA1: Skips the next instruction if the key stored in VX is pressed
                    if (chip->key[chip->V[(chip->opcode & 0x0F00) >> 8]] == 1)
                        chip->pc += 4;
                    else
                        chip->pc += 2;
                break;
                    
                default:
                    printf("Unknown opcode [0xE000]: 0x%X\n", chip->opcode);
            }
        break;

        case 0xF000:
            switch (chip->opcode & 0x000F) {
                case 0x0003:  // 0xFX33: stores the binary-coded decimal representation of VS, hungreds in I, tens I+1, ones I+2
                    chip->memory[chip->I]     = chip->V[(chip->opcode & 0x0F00) >> 8] / 100;
                    chip->memory[chip->I + 1] = (chip->V[(chip->opcode & 0x0F00) >> 8] / 10) % 10;
                    chip->memory[chip->I + 2] = (chip->V[(chip->opcode & 0x0F00) >> 8] % 100) % 10;
                    chip->pc += 2;
                break;

                case 0x0005:
                    switch (chip->opcode & 0x00F0) {
                        case 0x0050:  // 0xFX55: Stores from V0 to VX starting at I
                            for (short reg = 0; reg < (chip->opcode & 0x0F00) >> 8; reg++)
                                chip->memory[chip->I + reg] = chip->V[reg];
                            chip->pc += 2;
                        break;

                        case 0x0060:  // 0xFX65: Fills from V0 to VX with values from mem starting at I
                            for (short reg = 0; reg < (chip->opcode & 0x0F00) >> 8; reg++)
                                chip->V[reg] = chip->memory[chip->I + reg];
                            chip->pc += 2;
                        break;

                        case 0x0010:  // 0xFX15: Sets the delay timer to VX
                            chip->delay_timer = chip->V[(chip->opcode & 0x0F00) >> 8];
                            chip->pc += 2;
                        break;
                    }
                break;

                case 0x0007:  // 0xFX07: Sets VX to the value of the delay timer
                    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->delay_timer;
                    chip->pc += 2;
                break;

                case 0x0008:  // 0xFX18: Sets the sound timer to VX
                    chip->sound_timer = chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                break;

                case 0x0009:  // 0xFX29: Sets I to the location of the sprite for the character in VX
                    chip->I = chip->V[(chip->opcode & 0x0F00) >> 8] * 5;
                    chip->pc += 2;
                break;

                case 0x000A:  // 0xFX0A: A key press is awaited, and then stored in VX
                    short key_pressed = 0;
                    for (;;) {
                        for (short i = 0; i < 16; i++) {
                            if (chip->key[i] == 1) {
                                key_pressed = 1;
                                chip->V[(chip->opcode & 0x0F00) >> 8] = i;
                                break;
                            }
                        }
                        if (key_pressed == 1)
                            break;
                    }
                    chip->pc += 2;
                break;

                case 0x000E:  // FX1E: Adds VX to I
                    chip->I += chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                break;
            }
        break;
        default:
            printf("DECODING SUCKS!!!!!!\n");
        break;
    }
    
    // Update Timers
    if (chip->sound_timer > 0)
        chip->sound_timer--;
    if (chip->delay_timer > 0)
        chip->delay_timer--;
}

void my_chip8_load_game(chip8 *chip, FILE *game) {
    // Move the file pointer to the end and to get the file size
    fseek(game, 0, SEEK_END);
    long file_size = ftell(game);
    fseek(game, 0, SEEK_SET);  // Reset the file pointer to the beginning
    if (file_size > (4096 - 512)) {
        printf("Error: ROM size exceeds available memory.\n");
        fclose(game);
        exit(1);
    }

    // Read the entire file into memory
    size_t bytes_read = fread(&chip->memory[512], 1, file_size, game);

    if (bytes_read != file_size) {
        perror("Error reading file.");
        exit(1);
    }

    fclose(game);
    printf("Loaded %zu bytes into memory starting at 0x200\n", bytes_read);
}
