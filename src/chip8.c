#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// CHIP-8 font set. Each number is represented by a 5-byte sprite.
const uint8_t fontset = {
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

    // load font set into memory
    memcpy(&cpu->memory, fontset, FONTSET_SIZE);

    // set Program Counter to ROM start address
    cpu->pc = ROM_START_ADDRESS;

    // seed the random number generator
    srand(time(NULL));
}

// load ROM (assume SDL2 host)
bool chip8_load_rom(ChipIn_t* cpu, const char* filename) {
    FILE* rom_file = fopen(filename, "rb");
    if (!rom_file) {
        perror("Failed to open ROM");
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

    if (fread(&cpu->memory, 1, rom_size, rom_file)!= rom_size) {
        fprintf(stderr, "Failed to read ROM into memory.\n");
        fclose(rom_file);
        return false;
    }

    fclose(rom_file);
    return true;
}
