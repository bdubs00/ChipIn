#ifndef CHIP8_H
#define CHIP8_H

#endif //CHIP8_H

#ifndef CHIPIN_CHIP8_H
#define CHIPIN_CHIP8_H

#include <stdint.h>
#include <stdbool.h>

// constants
#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_DEPTH 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define NUM_KEYS 16
#define FONTSET_SIZE 80
#define ROM_START_ADDRESS 0x200

// main struct (encapsulates VM)
typedef struct {
    uint8_t memory;
    uint8_t V;
    uint16_t I;
    uint16_t pc;

    uint16_t stack;
    uint8_t sp;

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t keypad;
    uint32_t video;

    bool draw_flag;
} ChipIn_t;

// public API
void chip8_init(ChipIn_t* cpu);
bool chip8_load_rom(ChipIn_t* cpu, const char* filename);
void chip8_execute_cycle(ChipIn_t* cpu);

#endif //CHIPIN_CHIP8_H
