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

// Complete CHIP-8 VM structure
typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[NUM_REGISTERS];     // V0-VF registers
    uint16_t I;                   // Index register
    uint16_t pc;                  // Program counter
    
    uint16_t stack[STACK_DEPTH];  // Stack for subroutines
    uint8_t sp;                   // Stack pointer
    
    uint8_t delay_timer;          // Delay timer
    uint8_t sound_timer;          // Sound timer
    
    uint8_t keypad[NUM_KEYS];     // Keypad state
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]; // Display buffer
    
    bool draw_flag;               // Screen update flag
} ChipIn_t;

// Function prototypes
void chip8_init(ChipIn_t* cpu);
bool chip8_load_rom(ChipIn_t* cpu, const char* filename);
void chip8_execute_cycle(ChipIn_t* cpu);

#endif //CHIP8_H
