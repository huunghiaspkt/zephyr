# EFZ samples

Companion code for the [EmbeddedFun Zephyr training](https://huunghiaspkt.github.io/docs/zephyr-training/how-to-start).
Every sample is a self-contained Zephyr application, tested on the
**EFZ-ESP32S3** training board and the **ESP32-S3-DevKitC**.

These samples ship inside the EmbeddedFun Zephyr fork, so they are already in
your workspace after `west init` + `west update` — at
`zephyr/samples/efz_samples/`.

## Layout

The folders mirror the training docs, ordered from first build to production work.

| Folder | Level | What's inside |
|---|---|---|
| [`01_how-to-start/`](./01_how-to-start) | Beginner | Your first build — Hello World |
| [`02_basic/`](./02_basic) | Basic | Devicetree overlays, threads, the WS2812 RGB LED |
| [`03_intermediate/`](./03_intermediate) | Intermediate | Devicetree, bindings, Kconfig, the SHT30 sensor, BLE, power management, a custom driver |
| [`04_advanced/`](./04_advanced) | Advanced | Wi-Fi shell, wake-on-UART (nRF52840) |
| [`05_ztest/`](./05_ztest) | Testing | ztest and pytest suites for `native_sim` |
| [`displays/`](./displays) | Hardware | TFT overlays for the EFZ board and a display alignment test |

## Build a sample

From your workspace folder, with the virtual environment active:

```bash
west build -p -b efz_esp32s3/esp32s3/procpu zephyr/samples/efz_samples/02_basic/ws2812
west flash
```

Use `-b esp32s3_devkitc/esp32s3/procpu` for the DevKitC.

To change a sample freely, copy it out of the Zephyr tree first:

```bash
mkdir -p devzone
cp -r zephyr/samples/efz_samples/01_how-to-start/hello_world devzone/hello_world
cd devzone/hello_world
west build -b efz_esp32s3/esp32s3/procpu .
```

## Conventions

```
<sample_name>/
├── CMakeLists.txt
├── prj.conf
├── sample.yaml              # boards it builds for (used by twister)
├── boards/                  # optional, per-board overlays
│   ├── efz_esp32s3_procpu.overlay
│   └── esp32s3_devkitc_procpu.overlay
└── src/
    └── main.c
```

Build every sample for both boards in one go:

```bash
west twister -T zephyr/samples/efz_samples \
  -p efz_esp32s3/esp32s3/procpu -p esp32s3_devkitc/esp32s3/procpu --build-only
```

## Community

Questions, show-and-tell, or just want to follow along? Join the Facebook
group: [EmbeddedFun](https://web.facebook.com/groups/1284448006528960).

## License

Samples derived from Zephyr's own `samples/` tree keep their original
Apache-2.0 license. New samples here are Apache-2.0 unless noted.
