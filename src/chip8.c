#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// CHIP-8 font set
const uint8_t fontset[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(ChipIn_t* cpu) {
    // reset all state
    memset(cpu, 0, sizeof(ChipIn_t));

    // load font set into memory (fonts start at address 0x50)
    memcpy(cpu->memory + 0x50, fontset, FONTSET_SIZE);

    // set Program Counter to ROM start address
    cpu->pc = ROM_START_ADDRESS;

    // seed the random number generator
    srand(time(NULL));
}

bool chip8_load_rom(ChipIn_t* cpu, const char* filename) {
    FILE* rom_file = fopen(filename, "rb");
    if (!rom_file) {
        perror("Failed to open ROM! :(");
        return false;
    }

    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    rewind(rom_file);

    if (rom_size > (MEMORY_SIZE - ROM_START_ADDRESS)) {
        fprintf(stderr, "ROM file size is too large.\n");
        fclose(rom_file);
        return false;
    }

    if (fread(&cpu->memory[ROM_START_ADDRESS], 1, rom_size, rom_file) != rom_size) {
        fprintf(stderr, "Failed to read ROM into memory.\n");
        fclose(rom_file);
        return false;
    }

    fclose(rom_file);
    return true;
}

void chip8_execute_cycle(ChipIn_t* cpu) {
    // fetch instruction
    uint16_t instruction = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];

    // increment the program counter
    cpu->pc += 2;

    // xxtract common instruction components
    uint8_t x = (instruction & 0x0F00) >> 8;
    uint8_t y = (instruction & 0x00F0) >> 4;
    uint8_t kk = instruction & 0x00FF;
    uint16_t nnn = instruction & 0x0FFF;
    uint8_t n = instruction & 0x000F;

    // decode and execute
    switch (instruction & 0xF000) {
        case 0x0000:
            switch (instruction) {
                case 0x00E0: // CLS - Clear screen
                    memset(cpu->video, 0, sizeof(cpu->video));
                    cpu->draw_flag = true;
                    break;
                case 0x00EE: // RET - Return from subroutine
                    cpu->sp--;
                    cpu->pc = cpu->stack[cpu->sp];
                    break;
                default:
                    // SYS nnn - Jump to machine code routine (ignored in modern interpreters)
                    break;
            }
            break;

        case 0x1000: // JP nnn - Jump to address nnn
            cpu->pc = nnn;
            break;

        case 0x2000: // CALL nnn - Call subroutine at nnn
            cpu->stack[cpu->sp] = cpu->pc;
            cpu->sp++;
            cpu->pc = nnn;
            break;

        case 0x3000: // SE Vx, kk - Skip next instruction if Vx == kk
            if (cpu->V[x] == kk) {
                cpu->pc += 2;
            }
            break;

        case 0x4000: // SNE Vx, kk - Skip next instruction if Vx != kk
            if (cpu->V[x] != kk) {
                cpu->pc += 2;
            }
            break;

        case 0x5000: // SE Vx, Vy - Skip next instruction if Vx == Vy
            if (cpu->V[x] == cpu->V[y]) {
                cpu->pc += 2;
            }
            break;

        case 0x6000: // LD Vx, kk - Set Vx = kk
            cpu->V[x] = kk;
            break;

        case 0x7000: // ADD Vx, kk - Set Vx = Vx + kk
            cpu->V[x] += kk;
            break;

        case 0x8000:
            switch (n) {
                case 0x0: // LD Vx, Vy - Set Vx = Vy
                    cpu->V[x] = cpu->V[y];
                    break;
                case 0x1: // OR Vx, Vy - Set Vx = Vx OR Vy
                    cpu->V[x] |= cpu->V[y];
                    break;
                case 0x2: // AND Vx, Vy - Set Vx = Vx AND Vy
                    cpu->V[x] &= cpu->V[y];
                    break;
                case 0x3: // XOR Vx, Vy - Set Vx = Vx XOR Vy
                    cpu->V[x] ^= cpu->V[y];
                    break;
                case 0x4: // ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
                    {
                        uint16_t sum = cpu->V[x] + cpu->V[y];
                        uint8_t carry = (sum > 0xFF) ? 1 : 0;
                        cpu->V[x] = sum & 0xFF;
                        cpu->V[0xF] = carry;
                    }
                    break;
                case 0x5: // SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
                    cpu->V[0xF] = (cpu->V[x] > cpu->V[y]) ? 1 : 0;
                    cpu->V[x] -= cpu->V[y];
                    break;
                case 0x6: // SHR Vx - Set Vx = Vx SHR 1
                    cpu->V[0xF] = cpu->V[x] & 0x1;
                    cpu->V[x] >>= 1;
                    break;
                case 0x7: // SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
                    cpu->V[0xF] = (cpu->V[y] > cpu->V[x]) ? 1 : 0;
                    cpu->V[x] = cpu->V[y] - cpu->V[x];
                    break;
                case 0xE: // SHL Vx - Set Vx = Vx SHL 1
                    cpu->V[0xF] = (cpu->V[x] & 0x80) >> 7;
                    cpu->V[x] <<= 1;
                    break;
            }
            break;

        case 0x9000: // SNE Vx, Vy - Skip next instruction if Vx != Vy
            if (cpu->V[x] != cpu->V[y]) {
                cpu->pc += 2;
            }
            break;

        case 0xA000: // LD I, nnn - Set I = nnn
            cpu->I = nnn;
            break;

        case 0xB000: // JP V0, nnn - Jump to location nnn + V0
            cpu->pc = nnn + cpu->V[0];
            break;

        case 0xC000: // RND Vx, kk - Set Vx = random byte AND kk
            cpu->V[x] = (rand() % 256) & kk;
            break;

        case 0xD000: // DRW Vx, Vy, n - Draw n-byte sprite at (Vx, Vy)
            {
                uint8_t x_pos = cpu->V[x] % VIDEO_WIDTH;
                uint8_t y_pos = cpu->V[y] % VIDEO_HEIGHT;
                cpu->V[0xF] = 0;

                for (int row = 0; row < n; row++) {
                    uint8_t sprite_byte = cpu->memory[cpu->I + row];
                    for (int col = 0; col < 8; col++) {
                        uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
                        uint32_t* screen_pixel = &cpu->video[(y_pos + row) * VIDEO_WIDTH + (x_pos + col)];

                        if (sprite_pixel) {
                            if (*screen_pixel == 0xFFFFFFFF) {
                                cpu->V[0xF] = 1;
                            }
                            *screen_pixel ^= 0xFFFFFFFF;
                        }
                    }
                }
                cpu->draw_flag = true;
            }
            break;

        case 0xE000:
            switch (kk) {
                case 0x9E: // SKP Vx - Skip next instruction if key Vx is pressed
                    if (cpu->keypad[cpu->V[x]]) {
                        cpu->pc += 2;
                    }
                    break;
                case 0xA1: // SKNP Vx - Skip next instruction if key Vx is not pressed
                    if (!cpu->keypad[cpu->V[x]]) {
                        cpu->pc += 2;
                    }
                    break;
            }
            break;

        case 0xF000:
            switch (kk) {
                case 0x07: // LD Vx, DT - Set Vx = delay timer
                    cpu->V[x] = cpu->delay_timer;
                    break;
                case 0x0A: // LD Vx, K - Wait for key press, store key in Vx
                    {
                        bool key_pressed = false;
                        for (int i = 0; i < NUM_KEYS; i++) {
                            if (cpu->keypad[i]) {
                                cpu->V[x] = i;
                                key_pressed = true;
                                break;
                            }
                        }
                        if (!key_pressed) {
                            cpu->pc -= 2; // Stay on this instruction
                        }
                    }
                    break;
                case 0x15: // LD DT, Vx - Set delay timer = Vx
                    cpu->delay_timer = cpu->V[x];
                    break;
                case 0x18: // LD ST, Vx - Set sound timer = Vx
                    cpu->sound_timer = cpu->V[x];
                    break;
                case 0x1E: // ADD I, Vx - Set I = I + Vx
                    cpu->I += cpu->V[x];
                    break;
                case 0x29: // LD F, Vx - Set I = location of sprite for digit Vx
                    cpu->I = 0x50 + (cpu->V[x] * 5);
                    break;
                case 0x33: // LD B, Vx - Store BCD representation of Vx
                    cpu->memory[cpu->I] = cpu->V[x] / 100;
                    cpu->memory[cpu->I + 1] = (cpu->V[x] / 10) % 10;
                    cpu->memory[cpu->I + 2] = cpu->V[x] % 10;
                    break;
                case 0x55: // LD [I], Vx - Store V0 through Vx in memory starting at I
                    for (int i = 0; i <= x; i++) {
                        cpu->memory[cpu->I + i] = cpu->V[i];
                    }
                    break;
                case 0x65: // LD Vx, [I] - Read V0 through Vx from memory starting at I
                    for (int i = 0; i <= x; i++) {
                        cpu->V[i] = cpu->memory[cpu->I + i];
                    }
                    break;
            }
            break;
    }

    // update delay and sound timers
    if (cpu->delay_timer > 0) {
        cpu->delay_timer--;
    }

    if (cpu->sound_timer > 0) {
        cpu->sound_timer--;
    }
}