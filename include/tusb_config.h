/*
 * TinyUSB Configuration for hid-scroller
 * Minimal config - keyboard device only
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUSB_MCU                OPT_MCU_RP2040
#define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE
#define CFG_TUSB_OS                 OPT_OS_NONE

// Enable device stack
#define CFG_TUD_ENABLED             1

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

// Buffer size (keyboard doesn't need much)
#define CFG_TUD_ENDPOINT0_SIZE      64

// Device class drivers - keyboard only!
#define CFG_TUD_HID                 1
#define CFG_TUD_CDC                 0
#define CFG_TUD_MSC                 0
#define CFG_TUD_MIDI                0
#define CFG_TUD_VENDOR              0

// HID buffer size
#define CFG_TUD_HID_EP_BUFSIZE      16

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
