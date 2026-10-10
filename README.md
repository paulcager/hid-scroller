# HID Scroller

Automatic keyboard scroller for Raspberry Pi Pico. Sends PageUp (or any configured key) at random intervals (default: every 5-7 seconds).

## Use Case

Scrolling backwards through infinite-scroll web pages (chat logs, social media feeds, forums) without touching the keyboard.

## Features

- ✅ Plug-and-play USB HID keyboard (no drivers needed)
- ✅ Configurable random interval between keypresses (less bot-like than a fixed rate)
- ✅ Configurable key to send (PageUp, PageDown, arrows, etc.)
- ✅ Single-file implementation (~200 lines)
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

Settings are CMake options. Each keypress waits a random time between `INTERVAL_MIN_MS` and `INTERVAL_MAX_MS`.

| Option | Default | Meaning |
|--------|---------|---------|
| `INTERVAL_MIN_MS` | `5000` | Minimum milliseconds between keypresses |
| `INTERVAL_MAX_MS` | `7000` | Maximum milliseconds between keypresses |
| `KEY_TO_SEND` | `HID_KEY_PAGEUP` | Key to send |

Pass them when configuring a manual build:

```bash
cmake -DINTERVAL_MIN_MS=3000 -DINTERVAL_MAX_MS=5000 -DKEY_TO_SEND=HID_KEY_PAGEDOWN ..
```

or change the defaults in `CMakeLists.txt` (needed for `./build.sh`, which doesn't pass options through). Editing the `#define`s in `src/hid_scroller.c` has no effect, because CMake always supplies these values.

For a fixed interval, set `INTERVAL_MIN_MS` and `INTERVAL_MAX_MS` to the same value.

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

For Pico 2 (RP2350), use a separate build directory:

```bash
mkdir build.pico2 && cd build.pico2
cmake -DPICO_BOARD=pico2 ..
make -j$(nproc)
```

## Debug Output

The startup banner (version, interval, key) and a status line every 10 keys are printed on UART0: GP0 (TX) / GP1 (RX), 115200 baud. Nothing is sent over USB serial.

## Hardware

- **Raspberry Pi Pico** (RP2040) - recommended
- **Raspberry Pi Pico W** - also works (WiFi unused)
- Any RP2040 board should work
- **Raspberry Pi Pico 2** (RP2350) - builds, but untested on hardware

The device identifies as USB VID `0x2E8A` / PID `0xB90B`. The PID is unregistered; if you distribute this, request your own from Raspberry Pi (https://github.com/raspberrypi/usb-pid).

## Tips

**Too fast?** Increase `INTERVAL_MIN_MS` / `INTERVAL_MAX_MS` (e.g., 8000 / 10000)

**Too slow?** Decrease them (e.g., 2000 / 3000)

**Want to pause?** Just unplug the device or focus a different window

**Need different key?** Set `KEY_TO_SEND` (see `hid_keycodes.h`); the UART startup banner shows which key was built in

## License

MIT License (same as hid-proxy parent project)
