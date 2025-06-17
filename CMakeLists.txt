cmake_minimum_required(VERSION 3.13)

option(BUILD_FOR_PICO "Build firmware for Raspberry Pi Pico" OFF)

# bootstrap

if(BUILD_FOR_PICO)
    if(NOT DEFINED PICO_SDK_PATH)
        if(DEFINED ENV{PICO_SDK_PATH})
            set(PICO_SDK_PATH $ENV{PICO_SDK_PATH} CACHE PATH "Location of the Pico SDK")
        else()
            # fallback paths
            if(WIN32)
                set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk"
                        CACHE PATH "Location of the Pico SDK")
            elseif(UNIX AND NOT APPLE)
                set(PICO_SDK_PATH "/usr/share/pico-sdk"
                        CACHE PATH "Location of the Pico SDK")
            elseif(APPLE)
                set(PICO_SDK_PATH "/opt/pico-sdk"
                        CACHE PATH "Location of the Pico SDK")
            endif()
        endif()
    endif()

    # verify the SDK path exists
    if(NOT EXISTS "${PICO_SDK_PATH}")
        message(FATAL_ERROR "PICO_SDK_PATH not found: ${PICO_SDK_PATH}\n"
                "Please set the PICO_SDK_PATH environment variable or CMake cache variable")
    endif()

    message(STATUS "Using Pico SDK at: ${PICO_SDK_PATH}")

    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
    set(LANG_LIST C CXX ASM)
else()
    set(LANG_LIST C CXX)
endif()

project(ChipIn LANGUAGES ${LANG_LIST})
set(CMAKE_C_STANDARD 11)

if(BUILD_FOR_PICO)
    pico_sdk_init()

    add_executable(chipin_pico
            src/main_pico.c
            src/chip8.c
            src/hal_pico.c
            src/chip8.h)

    target_link_libraries(chipin_pico
            pico_stdlib
            hardware_pwm)

    pico_add_extra_outputs(chipin_pico)

else()
    find_package(SDL2 REQUIRED)

    add_executable(chipin_desktop
            src/main_sdl.c
            src/chip8.c
            src/hal_sdl.c
            src/chip8.h)

    # link against both SDL2 and SDL2main
    if(WIN32)
        target_link_libraries(chipin_desktop PRIVATE
                SDL2::SDL2main
                SDL2::SDL2)
    else()
        target_link_libraries(chipin_desktop PRIVATE SDL2::SDL2)
    endif()

    # ensure console subsystem on Windows
    if(WIN32 AND MINGW)
        set_target_properties(chipin_desktop PROPERTIES
                LINK_FLAGS "-mconsole")
    endif()
endif()