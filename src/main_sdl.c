#include "chip8.h"
#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ROM file>\n", argv[0]);
        return 1;
    }

    // initialize HAL
    hal_init();

    // initialize CHIP-8 system
    ChipIn_t chip8;
    chip8_init(&chip8);

    // load ROM
    if (!chip8_load_rom(&chip8, argv[1])) {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        hal_cleanup();
        return 1;
    }

    printf("CHIP-8 Emulator Started\n");
    printf("ROM loaded: %s\n", argv[1]);
    printf("Controls:\n");
    printf("  CHIP-8 Key -> Keyboard\n");
    printf("  1,2,3,C    -> 1,2,3,4\n");
    printf("  4,5,6,D    -> Q,W,E,R\n");
    printf("  7,8,9,E    -> A,S,D,F\n");
    printf("  A,0,B,F    -> Z,X,C,V\n");
    printf("  ESC        -> Quit\n\n");
    printf("  Have fun!! :) \n\n");

    // main emulation loop
    const int CYCLES_PER_FRAME = 10;  // adjust this to control emulation speed
    uint32_t last_time = SDL_GetTicks();

    while (!hal_should_quit()) {
        uint32_t current_time = SDL_GetTicks();

        // run emulation cycles
        for (int i = 0; i < CYCLES_PER_FRAME; i++) {
            // update keypad state
            hal_get_keypad_state(chip8.keypad);

            // execute one cycle
            chip8_execute_cycle(&chip8);
        }

        // draw screen if needed
        if (chip8.draw_flag) {
            hal_draw_screen(chip8.video);
            chip8.draw_flag = false;
        }

        // target ~60 FPS
        uint32_t frame_time = SDL_GetTicks() - current_time;
        if (frame_time < 16) {  // 16ms
            SDL_Delay(16 - frame_time);
        }
    }

    printf("Emulator shutting down...\n");
    hal_cleanup();
    return 0;
}