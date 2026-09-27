# 03 — Intermediate

Subsystem deep-dives. Samples here pair directly with the explainer pages
in `zephyr-training/03_intermediate/` — read the doc, then run the sample.

## Planned samples

| Sample | Source page | What it shows |
|---|---|---|
| `devicetree_tour/` | `devicetree.md` | Reading nodes, properties, and aliases at runtime |
| `binding_yaml/` | `binding-yaml.md` | A custom binding + a node that uses it |
| `kconfig_demo/` | `kconfig.md` | Enabling subsystems via `prj.conf` and per-board `*.conf` |
| `threads_sync/` | `threads.md` | Mutex, semaphore, and message queue between threads |
| `ble_basics/` | `ble-basics.md` | Minimal advertising + connection peripheral |
| `i2c_sensor/` | `i2c-sensors.md` | Reading a sensor via Zephyr's sensor API |
| `power_management/` | `power-management.md` | Idle states, device PM, and `pm_device` |
| `custom_driver/` | `writing-drivers.md` | A small out-of-tree driver + its binding |
| `layers_demo/` | `zephyr-layers.md` | Walking down: app → subsystem → driver → HAL |
| `common_mistakes/` | `common-mistakes.md` | Worked examples of the top gotchas |

## Prerequisites

- Comfortable building and flashing samples from `02_basic/`
- Some samples need extra hardware (BLE-capable MCU, an I²C sensor, etc.) —
  noted in the sample's own README.
