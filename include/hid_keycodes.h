/*
 * HID Keycode Definitions
 * From USB HID Usage Tables 1.12 Section 10: Keyboard/Keypad Page
 */

#ifndef HID_KEYCODES_H
#define HID_KEYCODES_H

// Commonly used keys for scrolling
#define HID_KEY_PAGEUP          0x4B
#define HID_KEY_PAGEDOWN        0x4E
#define HID_KEY_HOME            0x4A
#define HID_KEY_END             0x4D
#define HID_KEY_ARROW_UP        0x52
#define HID_KEY_ARROW_DOWN      0x51
#define HID_KEY_ARROW_LEFT      0x50
#define HID_KEY_ARROW_RIGHT     0x4F

// Letters (if you want to send A-Z)
#define HID_KEY_A               0x04
#define HID_KEY_B               0x05
#define HID_KEY_C               0x06
#define HID_KEY_D               0x07
#define HID_KEY_E               0x08
#define HID_KEY_F               0x09
#define HID_KEY_Z               0x1D

// Numbers (top row)
#define HID_KEY_1               0x1E
#define HID_KEY_2               0x1F
#define HID_KEY_3               0x20
#define HID_KEY_9               0x26
#define HID_KEY_0               0x27

// Special keys
#define HID_KEY_ENTER           0x28
#define HID_KEY_ESCAPE          0x29
#define HID_KEY_BACKSPACE       0x2A
#define HID_KEY_TAB             0x2B
#define HID_KEY_SPACE           0x2C

// Modifier key masks (for modifier byte)
#define HID_MOD_LCTRL           0x01
#define HID_MOD_LSHIFT          0x02
#define HID_MOD_LALT            0x04
#define HID_MOD_LGUI            0x08  // Windows/Command key
#define HID_MOD_RCTRL           0x10
#define HID_MOD_RSHIFT          0x20
#define HID_MOD_RALT            0x40
#define HID_MOD_RGUI            0x80

#endif /* HID_KEYCODES_H */
