# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a **minimal USB HID keyboard device** for Raspberry Pi Pico that automatically sends PageUp keypresses at regular intervals. Primary use case: scrolling backwards through infinite-scroll web pages (chat logs, social media feeds, etc.).

**Key Features:**
- Plug-and-play USB HID keyboard (no drivers needed)
- Configurable interval between keypresses
- Configurable key to send (default: PageUp)
- Single-file implementation (~150 lines)
- No flash storage, no encryption, no WiFi - just sends keys!

**Hardware:**
- Raspberry Pi Pico (RP2040) - **recommended**
- Raspberry Pi Pico W also works (WiFi unused)
- Raspberry Pi Pico2 (RP2350) - may work but untested

## Architecture

**Single-Core Design:**
- Everything runs on Core 0 in main loop
- TinyUSB device stack (acts as keyboard to host computer)
- Simple timer-based state machine

**No external dependencies** beyond:
- Pico SDK (including hardware RNG support)
- TinyUSB (included in Pico SDK)

## Configuration

Edit `src/hid_scroller.c` to change:

```c
#define INTERVAL_MIN_MS 5000    // Minimum milliseconds between keypresses
#define INTERVAL_MAX_MS 7000    // Maximum milliseconds between keypresses
#define KEY_TO_SEND HID_KEY_PAGEUP  // Key to send (see hid_keycodes.h)
```

The device uses the RP2040's hardware random number generator to pick a random interval between `INTERVAL_MIN_MS` and `INTERVAL_MAX_MS` for each keypress, making the timing more natural and less bot-like.

**Available keys:**
- `HID_KEY_PAGEUP` - Scroll up one page (default)
- `HID_KEY_PAGEDOWN` - Scroll down one page
- `HID_KEY_ARROW_UP` - Up arrow
- `HID_KEY_ARROW_DOWN` - Down arrow
- `HID_KEY_HOME` - Jump to top
- `HID_KEY_END` - Jump to bottom

See full list in `include/hid_keycodes.h`.

## Building

### Quick Build with Docker
```bash
./build.sh              # Build for Pico (default)
./build.sh --clean      # Clean build
```

### Manual Build (without Docker)
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Flashing
1. Hold BOOTSEL button on Pico while plugging in USB
2. Copy `build/hid_scroller.uf2` to the RPI-RP2 drive
3. Device reboots and starts sending keys immediately

## Usage

1. Flash firmware to Pico
2. Plug into USB port
3. Navigate to infinite-scroll webpage
4. Focus browser window
5. Device automatically sends PageUp every 5-7 seconds (random interval)

**To stop:** Unplug the device or close the browser tab.

**To pause temporarily:** Focus a different window/app (keys won't affect unfocused browser).

## File Structure

```
hid-scroller/
├── CMakeLists.txt          # Build configuration
├── pico_sdk_import.cmake   # Pico SDK integration
├── build.sh                # Docker build script
├── CLAUDE.md               # This file
├── README.md               # User documentation
├── include/
│   ├── hid_keycodes.h      # HID keycode definitions
│   └── tusb_config.h       # TinyUSB configuration
└── src/
    ├── hid_scroller.c      # Main application (single file!)
    └── usb_descriptors.c   # USB HID descriptors
```

## Code Organization

**src/hid_scroller.c** (~200 lines):
- `main()` - Initialize USB, enter main loop
- `get_random_interval()` - Generate random delay using hardware RNG
- `update_state()` - Timer-based state machine for key sending
- `send_key_report()` - Send HID keyboard report to host
- `tud_hid_report_complete_cb()` - TinyUSB callback

**src/usb_descriptors.c** (~100 lines):
- Minimal USB HID keyboard descriptor
- Device/product strings

**include/tusb_config.h** (~30 lines):
- TinyUSB configuration (keyboard only, no mouse/CDC)

## Extending

**Want to change the random interval range?**
```c
#define INTERVAL_MIN_MS 3000    // Faster: 3-5 seconds
#define INTERVAL_MAX_MS 5000
```

**Want to send multiple keys in sequence?**
```c
uint8_t sequence[] = {HID_KEY_PAGEUP, HID_KEY_PAGEUP, HID_KEY_PAGEUP};
// Send each key with delay between them
```

**Want to add a button to pause/resume?**
```c
#include "hardware/gpio.h"
// Check button state in main loop
if (gpio_get(BUTTON_PIN)) {
    // Skip sending key this iteration
}
```

**Want adaptive intervals (speed up over time)?**
```c
// In get_random_interval():
uint32_t min = INTERVAL_MIN_MS - (keys_sent * 100);  // Decrease by 100ms per key
if (min < 1000) min = 1000;  // Minimum 1 second
```

## Troubleshooting

**Keys not being sent:**
- Check browser window has focus
- Try unplugging/replugging device
- Check USB connection is good

**Sending too fast/slow:**
- Adjust `INTERVAL_MIN_MS` and `INTERVAL_MAX_MS` in source
- Rebuild and reflash

**Want fixed intervals instead of random:**
- Set `INTERVAL_MIN_MS` and `INTERVAL_MAX_MS` to the same value
- Or modify `get_random_interval()` to return a constant

**Wrong key being sent:**
- Change `KEY_TO_SEND` in source
- Rebuild and reflash

## Differences from hid-proxy

This project is intentionally **minimal** compared to hid-proxy:

| Feature | hid-scroller | hid-proxy |
|---------|--------------|-----------|
| Purpose | Send keys automatically | Intercept/modify keystrokes |
| Code size | ~150 lines | ~3000+ lines |
| Complexity | Single file | 20+ files |
| USB mode | Device only | Device + Host (PIO-USB) |
| Cores used | 1 (Core 0) | 2 (Core 0 + 1) |
| Storage | None | Encrypted flash storage |
| WiFi | None | HTTP server, MQTT |
| User input | None | Physical keyboard passthrough |

If you need macro expansion, encryption, or keyboard passthrough, use hid-proxy instead!

## License

Same as hid-proxy - MIT license (copy from there if you want to publish this).
