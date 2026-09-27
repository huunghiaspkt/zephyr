/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * WS2812 sample for ESP32-S3-DevKitC.
 *
 * The DevKitC has a single addressable WS2812 RGB LED on GPIO48, driven via
 * I2S. The board overlay (boards/esp32s3_devkitc_procpu.overlay) wires up
 * the I2S pinmux, declares the LED strip, and exposes it as `led-strip`.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ws2812_sample, LOG_LEVEL_INF);

#define STRIP_NODE        DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS  DT_PROP(STRIP_NODE, chain_length)

#define BRIGHTNESS  CONFIG_SAMPLE_LED_BRIGHTNESS
#define STEP_DELAY  K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb palette[] = {
	RGB(BRIGHTNESS, 0, 0),
	RGB(0, BRIGHTNESS, 0),
	RGB(0, 0, BRIGHTNESS),
	RGB(BRIGHTNESS, BRIGHTNESS, 0),
	RGB(0, BRIGHTNESS, BRIGHTNESS),
	RGB(BRIGHTNESS, 0, BRIGHTNESS),
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

int main(void)
{
	if (!device_is_ready(strip)) {
		LOG_ERR("LED strip %s not ready", strip->name);
		return 0;
	}

	LOG_INF("Cycling %u WS2812 pixel(s) on %s", STRIP_NUM_PIXELS, strip->name);

	size_t color = 0;

	while (1) {
		for (size_t i = 0; i < STRIP_NUM_PIXELS; i++) {
			pixels[i] = palette[color];
		}

		int rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
		if (rc) {
			LOG_ERR("update failed: %d", rc);
		}

		color = (color + 1) % ARRAY_SIZE(palette);
		k_sleep(STEP_DELAY);
	}

	return 0;
}
