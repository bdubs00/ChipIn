cmake_minimum_required(VERSION 3.13)

project(ChipIn LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)

add_executable(chipin
        src/main.c
        src/chip8.c
        src/chip8.h)
