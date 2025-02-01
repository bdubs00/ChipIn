#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

// CHIP-8 System constants
#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define NUM_KEYS 16
#define FONTSET_SIZE 80
#define ROM_START_ADDRESS 0x200

// Basic CHIP-8 VM structure
typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[NUM_REGISTERS];  // V0-VF registers
    uint16_t I;                // Index register
    uint16_t pc;               // Program counter
} ChipIn_t;

#endif //CHIP8_H
