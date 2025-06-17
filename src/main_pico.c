#include "chip8.h"
#include "pico/stdlib.h"
#include <stdio.h>

// test ROM data
static const uint8_t test_rom[] = {
    0xA2, 0x2A, // LD I, 0x22A (point to sprite data)
    0x60, 0x0C, // LD V0, 0x0C (X position)
    0x61, 0x08, // LD V1, 0x08 (Y position)
    0xD0, 0x1F, // DRW V0, V1, 15 (draw sprite)
    0x12, 0x00, // JP 0x200 (infinite loop)

    // sprite data (simple pattern)
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
};

// function to load ROM data into memory (since we don't have file system)
bool load_test_rom(ChipIn_t* cpu) {
    if (sizeof(test_rom) > (MEMORY_SIZE - ROM_START_ADDRESS)) {
        printf("Test ROM too large\n");
        return false;
    }

    // copy test ROM into memory
    for (size_t i = 0; i < sizeof(test_rom); i++) {
        cpu->memory[ROM_START_ADDRESS + i] = test_rom[i];
    }

    printf("Test ROM loaded (%d bytes)\n", sizeof(test_rom));
    return true;
}

int main() {
    // initialize HAL
    hal_init();

    printf("CHIP-8 Pico Emulator Starting...\n");

    // initialize CHIP-8 system
    ChipIn_t chip8;
    chip8_init(&chip8);


    if (!load_test_rom(&chip8)) {
        //TODO: implement ROM embedding (or load from SD)
    }

    printf("CHIP-8 system initialized\n");
    printf("Starting emulation loop...\n");

    // main loop
    uint32_t last_time = to_ms_since_boot(get_absolute_time());
    const uint32_t CYCLES_PER_SECOND = 700;  // CHIP-8 typically runs at ~700Hz
    const uint32_t MS_PER_CYCLE = 1000 / CYCLES_PER_SECOND;

    while (!hal_should_quit()) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        // execute cycle at appropriate timing
        if (current_time - last_time >= MS_PER_CYCLE) {
            // update keypad state
            hal_get_keypad_state(chip8.keypad);

            // execute one cycle
            chip8_execute_cycle(&chip8);

            // handle sound
            extern void hal_make_sound(bool enable);
            hal_make_sound(chip8.sound_timer > 0);

            // draw screen if needed
            if (chip8.draw_flag) {
                hal_draw_screen(chip8.video);
                chip8.draw_flag = false;
            }

            last_time = current_time;
        }

        // small delay to prevent busy waiting
        sleep_ms(1);
    }

    printf("Emulator shutting down...\n");
    hal_cleanup();
    return 0;
}