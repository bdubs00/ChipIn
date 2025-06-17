# ChipIn

A portable CHIP-8 emulator that runs on both desktop (SDL2) and Raspberry Pi Pico!

## Features

- Complete CHIP-8 instruction set implementation
- Clean HAL design
- Configurable emulation speed
- Sound timer support (beep!)

## Building

This project uses CMake and can be built for either desktop or Raspberry Pi Pico.

### Desktop Build (SDL2)

Prerequisites:
- CMake 3.13+
- SDL2 development libraries
- C compiler (GCC, Clang, MSVC)

```bash
mkdir build
cd build
cmake ..
make
```

### Pico Build

Prerequisites:
- CMake 3.13+
- Pico SDK installed
- ARM toolchain

```bash
mkdir build
cd build
cmake -DBUILD_FOR_PICO=ON ..
make
```

The Pico SDK path is automatically detected from:
1. `PICO_SDK_PATH` environment variable
2. Default system locations:
   - Windows: `C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk`
   - Linux: `/usr/share/pico-sdk`
   - macOS: `/opt/pico-sdk`

## Usage

### Desktop Version

```bash
./chipin_desktop path/to/rom.ch8
```

Controls:
```
CHIP-8 Key -> Keyboard
1,2,3,C    -> 1,2,3,4
4,5,6,D    -> Q,W,E,R
7,8,9,E    -> A,S,D,F
A,0,B,F    -> Z,X,C,V
ESC        -> Quit
```

### Pico Version

1. Flash the generated `chipin_pico.uf2` to your Pico
2. Wire up your keypad to GPIO 0-15
3. Connect a buzzer to GPIO 28 for sound
4. The emulator runs a built-in test ROM (displays sprites)

The Pico version currently includes a simple test ROM that draws some digits on screen. For now, the display output is sent over serial for debugging - I plan on adding support for SSD1306 displays soon.

## Project Structure

```
src/
├── chip8.c         # Core emulation logic
├── chip8.h         # CHIP-8 system definitions
├── hal_sdl.c       # SDL2 HAL implementation
├── hal_pico.c      # Pico HAL implementation
├── main_sdl.c      # Desktop entry point
└── main_pico.c     # Pico entry point
```

## Configuration

The emulator runs at approximately 700Hz (700 cycles per second), which is typical for CHIP-8. You can adjust the speed by modifying:
- Desktop: `CYCLES_PER_FRAME` in `main_sdl.c`
- Pico: `CYCLES_PER_SECOND` in `main_pico.c`

## Hardware Notes for Pico

- **Keypad**: GPIO 0-15 with internal pull-ups (active low)
- **Sound**: GPIO 28 with PWM for ~440Hz tone
- **Display**: Not yet implemented - serial debug output for now

Button debouncing is included with a 50ms timeout to prevent ghost inputs.

## TODOs

- Implement display hardware support for Pico
- Add ROM loading from SD card for Pico version
- Implement Super CHIP-8 extensions
- Add save states
- Configuration file support

## Contributing

Feel free to submit issues or PRs!

## License

This project is released under the MIT License:

Copyright (c) 2025 Brandon Winston

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.