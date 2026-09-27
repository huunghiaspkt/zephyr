# Wake up over UART

## Overview

A small demo that shows a low-power BLE bridge that lives in **System OFF** and
only wakes when the host has something to say:

1. The board sleeps in System OFF, drawing almost nothing.
2. The host sends a `0x00` byte over UART. The byte's start bit pulls the RX
   line low, which the nRF52840 senses as a wake and boots the firmware.
3. The firmware advertises as `Wakeup-UART`. A phone connects (any Nordic UART
   Service app, e.g. **nRF Connect for Mobile**) and enables notifications.
4. The firmware sends a `READY` byte (`0x06`) back to the host: *stream now*.
5. Every byte the host sends over UART is forwarded to the phone, unchanged.
6. If the host goes quiet for **5 s**, or no phone connects within **30 s** of
   waking, the board goes back to System OFF.

The whole thing is three small source files:

- `src/main.c` — the lifecycle (wake, timers, sleep) and the wiring.
- `src/uart_bridge.c` — receive raw host bytes; send the `READY` token.
- `src/ble_bridge.c` — advertise, connect, forward bytes as NUS notifications.

## Wiring

None. The host link is `uart0`, the DK's on-board USB VCOM, so the single USB
cable you already use to flash the board is also the host serial port (typically
`/dev/ttyACM0` on Linux). 115200 8N1.

Because `uart0` is now the data link, logs are sent over **RTT** instead of the
serial console.

## Requirements

- An nRF52840 DK (`nrf52840dk/nrf52840`).
- A phone with nRF Connect for Mobile.

## Building and running

> **Note:** This sample must be built with the nRF Connect SDK (NCS), not
> upstream Zephyr.

```console
west build -b nrf52840dk/nrf52840 .
west flash
```

Then, on the host:

1. Open the DK's USB serial port and send a `0x00` byte to wake the board
   (e.g. `printf '\x00' > /dev/ttyACM0`).
2. Connect to `Wakeup-UART` from a NUS phone app and enable notifications.
3. Watch for the `0x06` READY byte, then type/send anything — it appears on
   the phone.
4. Stop sending; after 5 s the board sleeps again.

## Configuration

Timings and the READY token live in `Kconfig` / `prj.conf`:

- `CONFIG_APP_ADV_TIMEOUT_MS` — no-phone window (default 30000).
- `CONFIG_APP_UART_IDLE_MS` — UART-idle window (default 5000).
- `CONFIG_APP_READY_BYTE` — READY token (default 0x06).
