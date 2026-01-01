#!/bin/bash

# Build script for hid-scroller
# Uses Docker for consistent build environment

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

# Default options
CLEAN_BUILD=false
BOARD="pico"

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --board)
            BOARD="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --clean         Clean build directory before building"
            echo "  --board BOARD   Target board (default: pico)"
            echo "                  Options: pico, pico_w"
            echo "  --help          Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Run '$0 --help' for usage information"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Build with Docker
echo "Building hid-scroller for $BOARD..."
docker run --rm \
    -v "$SCRIPT_DIR:/home/builder/project" \
    -w /home/builder/project/build \
    ghcr.io/raspberrypi/pico-sdk:latest \
    bash -c "cmake -DPICO_BOARD=$BOARD .. && make -j\$(nproc)"

echo ""
echo "Build complete!"
echo "Output: $BUILD_DIR/hid_scroller.uf2"
echo ""
echo "To flash:"
echo "1. Hold BOOTSEL button on Pico while plugging in USB"
echo "2. Copy $BUILD_DIR/hid_scroller.uf2 to RPI-RP2 drive"
