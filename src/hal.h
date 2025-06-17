#ifndef CHIPIN_HAL_H
#define CHIPIN_HAL_H

#include <stdint.h>
#include <stdbool.h>

// a constant to represent that no key is being pressed.
#define HAL_NO_KEY_PRESSED 0xFF

void hal_init(void);

void hal_shutdown(void);

void hal_process_input(uint8_t keys[16]);

void hal_graphics_draw(const uint8_t* display_buffer);

void hal_sound_beep(bool play);

void hal_timing_delay(uint32_t ms);

typedef struct {
    bool     (*init)(void);
    void     (*shutdown)(void);
    void     (*handle_input)(uint8_t *keypad, bool *quit);
    void     (*render)(const uint32_t *video);
    void     (*beep)(bool play);
    uint32_t (*get_ticks)(void);
} Hal_t;


#endif //CHIPIN_HAL_H