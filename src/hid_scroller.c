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
#include "tusb.h"
#include "hid_keycodes.h"

//--------------------------------------------------------------------
// CONFIGURATION - Edit these to customize behavior
//--------------------------------------------------------------------

// How often to send a keypress (milliseconds)
#define INTERVAL_MS 1000

// Which key to send (see hid_keycodes.h for options)
#define KEY_TO_SEND HID_KEY_PAGEUP

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

//--------------------------------------------------------------------
// USB HID KEYBOARD REPORT
//--------------------------------------------------------------------

typedef struct {
    uint8_t modifier;   // Modifier keys (Ctrl, Shift, Alt, etc.)
    uint8_t reserved;   // Reserved (always 0)
    uint8_t keycode[6]; // Up to 6 simultaneous keys
} hid_keyboard_report_t;

//--------------------------------------------------------------------
// MAIN LOGIC
//--------------------------------------------------------------------

// Send a HID keyboard report to the host
static void send_key_report(uint8_t modifier, uint8_t keycode) {
    if (!tud_hid_ready()) {
        return;  // USB not ready yet
    }

    hid_keyboard_report_t report = {
        .modifier = modifier,
        .reserved = 0,
        .keycode = {keycode, 0, 0, 0, 0, 0}
    };

    tud_hid_keyboard_report(0, report.modifier, report.keycode);
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
            // Send the configured key
            send_key_report(0, KEY_TO_SEND);
            current_state = STATE_SEND_RELEASE;
            next_action_time = make_timeout_time_ms(KEY_HOLD_MS);
            keys_sent++;
            break;

        case STATE_SEND_RELEASE:
            // Release all keys
            send_key_report(0, 0);
            current_state = STATE_IDLE;
            next_action_time = make_timeout_time_ms(INTERVAL_MS);

            // Print status every 10 keys
            if (keys_sent % 10 == 0) {
                printf("Sent %lu keys (PageUp every %dms)\n",
                       (unsigned long)keys_sent, INTERVAL_MS);
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
    printf("Interval: %d ms\n", INTERVAL_MS);
    printf("Key: PageUp (0x%02X)\n", KEY_TO_SEND);
    printf("=================================\n");
    printf("\n");

    // Initialize TinyUSB
    tud_init(BOARD_TUD_RHPORT);

    // Schedule first action
    next_action_time = make_timeout_time_ms(INTERVAL_MS);

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
