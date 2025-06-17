#include "chip8.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <stdio.h>

// Pin definitions
#define BUTTON_PINS_START 0    // GPIO 0-15 for keypad
#define LED_MATRIX_START 16    // GPIO pins for LED matrix (if using shift registers)
#define BUZZER_PIN 28          // GPIO for buzzer/speaker

// simple button debouncing
static uint32_t last_button_time = 0;
static const uint32_t DEBOUNCE_TIME_MS = 50;

void hal_init(void) {
    stdio_init_all();

    // initialize keypad pins as inputs with pull-ups
    for (int i = 0; i < NUM_KEYS; i++) {
        gpio_init(BUTTON_PINS_START + i);
        gpio_set_dir(BUTTON_PINS_START + i, GPIO_IN);
        gpio_pull_up(BUTTON_PINS_START + i);
    }

    // initialize buzzer pin for sound
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    // set up PWM for buzzer
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_enabled(slice_num, true);

    printf("CHIP-8 Pico HAL initialized\n");
}

void hal_cleanup(void) {
    // Turn off buzzer
    gpio_put(BUZZER_PIN, 0);
    printf("CHIP-8 Pico HAL cleanup complete\n");
}

void hal_draw_screen(uint32_t* video_buffer) {
    // TODO: Implement based on whatever screen hardware I go with
    // for now, just output to serial for debugging
    static int frame_count = 0;
    if (frame_count % 60 == 0) {  // print every 60 frames
        printf("Frame %d - Display update\n", frame_count);

        // sample ASCII art
        for (int y = 0; y < VIDEO_HEIGHT; y += 4) {  // sample every 4th row
            for (int x = 0; x < VIDEO_WIDTH; x += 8) {  // sample every 8th column
                if (video_buffer[y * VIDEO_WIDTH + x] != 0) {
                    printf("*");
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }
        printf("\n");
    }
    frame_count++;
}

void hal_get_keypad_state(uint8_t* keypad) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // simple debouncing
    if (current_time - last_button_time < DEBOUNCE_TIME_MS) {
        return;  // too soon! use previous state
    }

    bool any_pressed = false;
    for (int i = 0; i < NUM_KEYS; i++) {
        // buttons are active low (pressed = 0 due to pull-up)
        keypad[i] = !gpio_get(BUTTON_PINS_START + i) ? 1 : 0;
        if (keypad[i]) {
            any_pressed = true;
        }
    }

    if (any_pressed) {
        last_button_time = current_time;
    }
}

bool hal_should_quit(void) {
    // for now, always return false to keep running
    return false;
}

// helper function to make sound (can be called when sound_timer > 0)
void hal_make_sound(bool enable) {
    if (enable) {
        // generate a simple square wave at ~440Hz (A note)
        uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
        pwm_set_wrap(slice_num, 1136);  // For ~440Hz at default clock
        pwm_set_chan_level(slice_num, PWM_CHAN_A, 568);  // 50% duty cycle
    } else {
        // turn off sound
        uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
        pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
    }
}