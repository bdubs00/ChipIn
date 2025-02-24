#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// CHIP-8 font set. Each character is 4x5 pixels
const uint8_t fontset[FONTSET_SIZE] = {
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
    // Clear all memory, registers and screen
    memset(cpu, 0, sizeof(ChipIn_t));

    // Load font set into memory starting at 0x50
    for (int i = 0; i < FONTSET_SIZE; i++) {
        cpu->memory[0x50 + i] = fontset[i];
    }

    // Set program counter to ROM start address
    cpu->pc = ROM_START_ADDRESS;

    // Initialize random seed
    srand(time(NULL));
}

bool chip8_load_rom(ChipIn_t* cpu, const char* filename) {
    FILE* rom_file = fopen(filename, "rb");
    if (!rom_file) {
        printf("Failed to open ROM file: %s\n", filename);
        return false;
    }

    // Get file size
    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    rewind(rom_file);

    // Check if ROM fits in memory
    if (rom_size > (MEMORY_SIZE - ROM_START_ADDRESS)) {
        printf("ROM file too large: %ld bytes\n", rom_size);
        fclose(rom_file);
        return false;
    }

    // Read ROM into memory
    size_t result = fread(&cpu->memory[ROM_START_ADDRESS], 1, rom_size, rom_file);
    if (result != rom_size) {
        printf("Failed to read ROM file\n");
        fclose(rom_file);
        return false;
    }

    fclose(rom_file);
    printf("Loaded ROM: %s (%ld bytes)\n", filename, rom_size);
    return true;
}
