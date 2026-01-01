# HID Scroller

Automatic keyboard scroller for Raspberry Pi Pico. Sends PageUp (or any configured key) at regular intervals.

## Use Case

Scrolling backwards through infinite-scroll web pages (chat logs, social media feeds, forums) without touching the keyboard.

## Features

- ✅ Plug-and-play USB HID keyboard (no drivers needed)
- ✅ Configurable interval between keypresses
- ✅ Configurable key to send (PageUp, PageDown, arrows, etc.)
- ✅ Single-file implementation (~150 lines)
- ✅ Minimal dependencies (just Pico SDK)

## Quick Start

```bash
# Build
./build.sh

# Flash to Pico
# 1. Hold BOOTSEL button while plugging in USB
# 2. Copy build/hid_scroller.uf2 to RPI-RP2 drive

# Use
# Just plug it in and navigate to a webpage - it starts scrolling automatically!
```

## Configuration

Edit `src/hid_scroller.c` and change these values:

```c
#define INTERVAL_MS 1000        // Milliseconds between keypresses (default: 1 second)
#define KEY_TO_SEND HID_KEY_PAGEUP  // Key to send (default: PageUp)
```

### Available Keys

See `include/hid_keycodes.h` for full list. Common options:

- `HID_KEY_PAGEUP` - Scroll up one page (default)
- `HID_KEY_PAGEDOWN` - Scroll down one page
- `HID_KEY_ARROW_UP` - Up arrow
- `HID_KEY_ARROW_DOWN` - Down arrow
- `HID_KEY_HOME` - Jump to top
- `HID_KEY_END` - Jump to bottom

## Building

### With Docker (recommended)
```bash
./build.sh              # Build for Pico
./build.sh --clean      # Clean build
./build.sh --board pico_w  # Build for Pico W
```

### Without Docker
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Hardware

- **Raspberry Pi Pico** (RP2040) - recommended
- **Raspberry Pi Pico W** - also works (WiFi unused)
- Any RP2040 board should work

## Tips

**Too fast?** Increase `INTERVAL_MS` (e.g., 2000 for 2 seconds)

**Too slow?** Decrease `INTERVAL_MS` (e.g., 500 for 0.5 seconds)

**Want to pause?** Just unplug the device or focus a different window

**Need different key?** Change `KEY_TO_SEND` (see `hid_keycodes.h`)

## License

MIT License (same as hid-proxy parent project)
