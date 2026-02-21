cmake_minimum_required(VERSION 3.13)

# Set Pico SDK path
if(NOT PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message(STATUS "Using PICO_SDK_PATH from environment: ${PICO_SDK_PATH}")
    else()
        set(PICO_SDK_PATH /home/paul/pico/pico-sdk)
        message(STATUS "Using default PICO_SDK_PATH: ${PICO_SDK_PATH}")
    endif()
endif()

include(pico_sdk_import.cmake)

project(hid_scroller C CXX ASM)

pico_sdk_init()

# Configuration options (can be overridden at build time)
set(INTERVAL_MIN_MS "5000" CACHE STRING "Minimum milliseconds between keypresses")
set(INTERVAL_MAX_MS "7000" CACHE STRING "Maximum milliseconds between keypresses")
set(KEY_TO_SEND "HID_KEY_PAGEUP" CACHE STRING "Key to send (e.g., HID_KEY_PAGEUP, HID_KEY_PAGEDOWN)")

message(STATUS "Configuration:")
message(STATUS "  INTERVAL_MIN_MS = ${INTERVAL_MIN_MS}")
message(STATUS "  INTERVAL_MAX_MS = ${INTERVAL_MAX_MS}")
message(STATUS "  KEY_TO_SEND = ${KEY_TO_SEND}")

# Capture git commit hash for version identification
execute_process(
    COMMAND git describe --always --dirty --abbrev=8
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH "unknown")
endif()
message(STATUS "Git commit: ${GIT_COMMIT_HASH}")

add_executable(hid_scroller
    src/hid_scroller.c
    src/usb_descriptors.c
)

target_include_directories(hid_scroller PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(hid_scroller PUBLIC
    pico_stdlib
    tinyusb_device
    tinyusb_board
)

target_compile_definitions(hid_scroller PRIVATE
    GIT_COMMIT_HASH="${GIT_COMMIT_HASH}"
    INTERVAL_MIN_MS=${INTERVAL_MIN_MS}
    INTERVAL_MAX_MS=${INTERVAL_MAX_MS}
    KEY_TO_SEND=${KEY_TO_SEND}
)

pico_add_extra_outputs(hid_scroller)

# Enable UART stdio for debugging
pico_enable_stdio_usb(hid_scroller 0)
pico_enable_stdio_uart(hid_scroller 1)
