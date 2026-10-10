/*
 * HID Scroller - Automatic Keyboard Scroller
 *
 * Sends PageUp (or other configured key) at regular intervals.
 * Use case: Scrolling backwards through infinite-scroll web pages.
 *
 * Hardware: Raspberry Pi Pico (RP2040)
 * Just plug in via USB - appears as a keyboard!
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "pico/rand.h"
#include "tusb.h"
#include "hid_keycodes.h"

//--------------------------------------------------------------------
// CONFIGURATION - Edit these to customize behavior
// (or override at build time with cmake -DINTERVAL_MIN_MS=3000 ...)
//--------------------------------------------------------------------

// Random interval range for keypresses (milliseconds)
#ifndef INTERVAL_MIN_MS
#define INTERVAL_MIN_MS 5000  // 5 seconds
#endif

#ifndef INTERVAL_MAX_MS
#define INTERVAL_MAX_MS 7000  // 7 seconds
#endif

// Which key to send (see hid_keycodes.h for options)
#ifndef KEY_TO_SEND
#define KEY_TO_SEND HID_KEY_PAGEUP
#endif

// Human-readable key name for log output (set by CMake from KEY_TO_SEND)
#ifndef KEY_TO_SEND_NAME
#define KEY_TO_SEND_NAME "HID_KEY_PAGEUP"
#endif

// How long to hold key down before releasing (milliseconds)
#define KEY_HOLD_MS 50

//--------------------------------------------------------------------
// STATE MACHINE
//--------------------------------------------------------------------

typedef enum {
    STATE_IDLE,         // Waiting for next interval
    STATE_SEND_KEY,     // Send key press
    STATE_SEND_RELEASE, // Send key release
} state_t;

static state_t current_state = STATE_IDLE;
static absolute_time_t next_action_time;
static uint32_t keys_sent = 0;
static uint32_t current_interval_ms = INTERVAL_MIN_MS;

//--------------------------------------------------------------------
// MAIN LOGIC
//--------------------------------------------------------------------

// Send a HID keyboard report to the host.
// Returns false if the report could not be queued (caller should retry).
static bool send_key_report(uint8_t modifier, uint8_t keycode) {
    if (!tud_hid_ready()) {
        return false;  // Endpoint busy or USB not ready
    }

    // Use TinyUSB's built-in keyboard report function
    uint8_t keycodes[6] = {keycode, 0, 0, 0, 0, 0};
    return tud_hid_keyboard_report(0, modifier, keycodes);
}

// Generate random interval between INTERVAL_MIN_MS and INTERVAL_MAX_MS
static uint32_t get_random_interval(void) {
    uint32_t range = INTERVAL_MAX_MS - INTERVAL_MIN_MS;
    uint32_t random_offset = get_rand_32() % (range + 1);
    return INTERVAL_MIN_MS + random_offset;
}

// Main state machine - called from main loop
static void update_state(void) {
    absolute_time_t now = get_absolute_time();

    // Don't do anything until it's time for next action
    if (absolute_time_diff_us(now, next_action_time) > 0) {
        return;
    }

    switch (current_state) {
        case STATE_IDLE:
            // Time to send another key
            current_state = STATE_SEND_KEY;
            next_action_time = make_timeout_time_ms(KEY_HOLD_MS);
            break;

        case STATE_SEND_KEY:
            // Send the configured key (retry on next loop if busy)
            if (!send_key_report(0, KEY_TO_SEND)) {
                break;
            }
            current_state = STATE_SEND_RELEASE;
            next_action_time = make_timeout_time_ms(KEY_HOLD_MS);
            keys_sent++;
            break;

        case STATE_SEND_RELEASE:
            // Release all keys - must not be dropped, or the host sees the
            // key held down and auto-repeats it. Retry on next loop if busy.
            if (!send_key_report(0, 0)) {
                break;
            }
            current_state = STATE_IDLE;

            // Generate new random interval for next key
            current_interval_ms = get_random_interval();
            next_action_time = make_timeout_time_ms(current_interval_ms);

            // Print status every 10 keys
            if (keys_sent % 10 == 0) {
                printf("Sent %lu keys (" KEY_TO_SEND_NAME " every %lu-%lums, next in %lums)\n",
                       (unsigned long)keys_sent,
                       (unsigned long)INTERVAL_MIN_MS,
                       (unsigned long)INTERVAL_MAX_MS,
                       (unsigned long)current_interval_ms);
            }
            break;
    }
}

//--------------------------------------------------------------------
// TINYUSB CALLBACKS
//--------------------------------------------------------------------

// Invoked when sent REPORT successfully to host
// We don't need to do anything here
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
    (void) instance;
    (void) report;
    (void) len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;  // No reports to send
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint (Report ID = 0, Type = 0)
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const* buffer, uint16_t bufsize) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;

    // We don't care about reports from host (e.g. LED status)
}

//--------------------------------------------------------------------
// MAIN ENTRY POINT
//--------------------------------------------------------------------

int main(void) {
    // Initialize Pico
    stdio_init_all();

    // Brief delay to let USB serial settle (for debug output)
    sleep_ms(100);

    printf("\n");
    printf("=================================\n");
    printf("HID Scroller starting...\n");
    printf("Version: " GIT_COMMIT_HASH "\n");
    printf("Interval: %d-%d ms (random)\n", INTERVAL_MIN_MS, INTERVAL_MAX_MS);
    printf("Key: " KEY_TO_SEND_NAME " (0x%02X)\n", KEY_TO_SEND);
    printf("=================================\n");
    printf("\n");

    // Initialize TinyUSB (use default port 0 for RP2040/RP2350)
    tud_init(0);

    // Schedule first action with random interval
    current_interval_ms = get_random_interval();
    next_action_time = make_timeout_time_ms(current_interval_ms);

    printf("USB initialized, waiting for host...\n");

    // Main loop
    while (true) {
        tud_task();  // TinyUSB device task

        // Only send keys when USB is mounted (connected to host)
        if (tud_mounted()) {
            update_state();
        }
    }

    return 0;
}
