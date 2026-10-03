.. zephyr:board:: efz_esp32s3

Overview
********

The EFZ-ESP32S3 is the EmbeddedFun training board: an ESP32-S3 together with the
parts an embedded course needs — a temperature and humidity sensor, a 6-axis IMU,
an RGB LED, a microphone, a speaker amplifier, a microSD slot, a display on the
back, and headers for your own wiring. It is powered, flashed and debugged over a
single USB-C cable. For the course that uses it, see `EmbeddedFun Zephyr training`_.

Hardware
********

ESP32-S3 Features
=================

ESP32-S3 is a low-power MCU-based system on a chip (SoC) with integrated 2.4 GHz
Wi-Fi and Bluetooth® Low Energy (Bluetooth LE). It consists of a high-performance
dual-core microprocessor (Xtensa® 32-bit LX7), a low power coprocessor, a Wi-Fi
baseband, a Bluetooth LE baseband, an RF module, and numerous peripherals.

- Dual core 32-bit Xtensa Microprocessor (Tensilica LX7), running up to 240MHz
- Additional vector instructions support for AI acceleration
- 512KB of SRAM
- 384KB of ROM
- Wi-Fi 802.11b/g/n
- Bluetooth LE 5.0 with long-range support and up to 2Mbps data rate

Digital interfaces:

- 45 programmable GPIOs
- 4x SPI, 3x UART, 2x I2C, 2x I2S
- LCD interface and DVP camera interface
- RMT (TX/RX), pulse counter, LED PWM controller (up to 8 channels), 2x MCPWM
- Full-speed USB OTG and USB Serial/JTAG controller
- SDIO host controller with 2 slots
- General DMA controller (GDMA), with 5 transmit channels and 5 receive channels
- TWAI® controller, compatible with ISO 11898-1 (CAN Specification 2.0)

Analog interfaces:

- 2x 12-bit SAR ADCs, up to 20 channels
- Temperature sensor
- 14x touch sensing IOs

Timers:

- 4x 54-bit general-purpose timers
- 52-bit system timer
- 3x watchdog timers

Low Power:

- Power Management Unit with five power modes
- Ultra-Low-Power (ULP) coprocessors: ULP-RISC-V and ULP-FSM

Security:

- Secure boot and flash encryption
- 4-Kbit OTP, up to 1792 bits for users
- Cryptographic hardware acceleration: AES-128/256, Hash, RSA, RNG, HMAC, digital signature

On this board the ESP32-S3 has no in-package flash or PSRAM: code runs from an
external 8 MB QSPI flash (EN25QH64A). For details, see the `ESP32-S3 Datasheet`_
and the `ESP32-S3 Technical Reference Manual`_.

Board Components
================

.. list-table::
   :header-rows: 1

   * - Feature
     - Detail
   * - MCU
     - ESP32-S3 (QFN56, no in-package flash or PSRAM)
   * - Flash
     - 8 MB external QSPI (EN25QH64A)
   * - Wireless
     - Wi-Fi 802.11 b/g/n, Bluetooth LE 5.0, chip antenna
   * - USB
     - USB-C to the native USB Serial/JTAG controller (flashing, console, JTAG)
   * - Power
     - 5 V USB to 3.3 V LDO, red power LED
   * - Sensors
     - SHT30 temperature/humidity (I2C ``0x44``), MPU-6500 6-axis IMU (I2C ``0x69``),
       NTC thermistor input
   * - Audio
     - INMP441 I2S microphone, MAX98357A I2S class-D amplifier
   * - Storage
     - microSD, 4-bit SDIO with card-detect
   * - User I/O
     - WS2812B RGB LED, 2x red LEDs, 2x user buttons, BOOT and RESET buttons
   * - Display
     - Rounded TFT on the back (SPI connector)
   * - Expansion
     - Two 7-pin headers; I2C, UART, NTC and speaker connectors

Supported Features
==================

.. zephyr:board-supported-hw::

Connections and IOs
===================

.. list-table::
   :header-rows: 1

   * - Function
     - Part
     - ESP32-S3 pins
   * - USB
     - Native USB Serial/JTAG
     - D- GPIO19, D+ GPIO20
   * - UART0
     - Box header P6
     - TX GPIO43, RX GPIO44
   * - I2C0
     - SHT30 ``0x44``, MPU-6500 ``0x69`` (INT GPIO48), box header P5
     - SDA GPIO17, SCL GPIO18
   * - ADC
     - NTC 10 kΩ B3950 (header P7), 10 kΩ pull-up to 3V3
     - GPIO1 (ADC1_CH0)
   * - RGB LED
     - WS2812B
     - GPIO21
   * - LEDs
     - LED1, LED2 (active-high)
     - GPIO46, GPIO45
   * - Buttons
     - BOOT, RESET; BN1, BN2 (active-low, 10 kΩ pull-up)
     - GPIO0, CHIP_PU; GPIO36, GPIO37
   * - microSD
     - 4-bit SDIO
     - CLK 9, CMD 10, D0 8, D1 7, D2 12, D3 11, CD 6
   * - Microphone
     - INMP441 (left channel)
     - BCLK 2, WS 4, DOUT 3, EN 5
   * - Speaker
     - MAX98357A (SD pulled low: amplifier off until driven)
     - BCLK 34, DATA 33, WS 35, SD 47
   * - TFT
     - SPI display connector (shared with header P4)
     - SCL 40, SDA 41, CS 39, DC 38, RST 42, BLK 16
   * - Header P4
     - 7-pin
     - 3V3, GPIO42, 41, 40, 39, 38, GND
   * - Header P3
     - 7-pin
     - 3V3, GPIO26, 16, 15, 14, 13, GND

Strapping pins
--------------

.. list-table::
   :header-rows: 1

   * - Pin
     - Controls
     - On this board
     - Result
   * - GPIO0
     - Boot mode
     - BOOT button, 10 kΩ pull-up
     - Runs from flash; hold BOOT for download mode
   * - GPIO46
     - Boot mode, ROM log
     - LED1 with 1 kΩ to GND
     - 0, as download mode requires
   * - GPIO45
     - VDD_SPI voltage
     - LED2 with 1 kΩ to GND
     - 0, so VDD_SPI is 3.3 V as the EN25QH64A flash needs
   * - GPIO3
     - JTAG source
     - INMP441 data out
     - Ignored unless the ``STRAP_JTAG_SEL`` eFuse is burned

.. warning::

   GPIO45 high at reset switches VDD_SPI to 1.8 V; the 3.3 V flash then cannot be
   read and the board does not boot.

Rev 1 errata
============

The silkscreen next to header P3 reads *3V3, GPIO16, 15, 14, 13, 12, GND*. The
schematic is correct: the pins are 3V3, GPIO26, 16, 15, 14, 13, GND.

References
**********

.. target-notes::

.. _`EmbeddedFun Zephyr training`: https://huunghiaspkt.github.io/docs/zephyr-training/basic/efz-esp32s3-board
.. _`ESP32-S3 Datasheet`: https://documentation.espressif.com/esp32-s3_datasheet_en.pdf
.. _`ESP32-S3 Technical Reference Manual`: https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf
