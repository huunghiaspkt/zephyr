# 02 — Basic

Board bring-up and first-real-feature samples. After this section you
should be comfortable with devicetree overlays, threads, and a simple
peripheral driver (WS2812).

## Planned samples

| Sample | Source page | What it shows |
|---|---|---|
| `ws2812/` | `zephyr-training/02_basic/ws2812.md` | Board bring-up + first peripheral — the on-board WS2812 RGB on GPIO48 (no plain user LED on this DevKitC) |
| `overlay_demo/` | `zephyr-training/02_basic/overlays.md` | Board-specific `.overlay` file overrides |
| `threads_basic/` | `zephyr-training/02_basic/threads.md` | Two cooperating threads with `k_msleep` |
| `finding_apis/` | `zephyr-training/02_basic/finding-apis.md` | Worked examples of locating drivers and headers |

> `esp32s3-board.md` is a "meet the board" reading page — no sample.
> The ESP32-S3-DevKitC has only an addressable WS2812 (GPIO48, via I2S),
> so the `ws2812/` sample doubles as the board bring-up check.

## Prerequisites

Finish `01_how-to-start/` first — these samples assume `west build` /
`west flash` already work on your board.
