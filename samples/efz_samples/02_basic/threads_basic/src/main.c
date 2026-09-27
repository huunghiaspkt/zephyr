/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

/* ── WS2812 setup ── */
#define STRIP_NODE DT_ALIAS(led_strip)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define DELAY_TIME K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00, 0x00), /* red   */
	RGB(0x00, CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00), /* green */
	RGB(0x00, 0x00, CONFIG_SAMPLE_LED_BRIGHTNESS), /* blue  */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

#define STACK_SIZE      1024
#define HELLO_PRIORITY  5
#define WS2812_PRIORITY 5

/* ── Thread 1: Hello EmbeddedFun (K_THREAD_DEFINE) ── */
void hello_thread_fn(void *a, void *b, void *c)
{
	int count = 0;

	while (1) {
		printf("Hello EmbeddedFun from %s count=%d\n", CONFIG_BOARD, count++);
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(hello_tid, STACK_SIZE, hello_thread_fn,
		NULL, NULL, NULL, HELLO_PRIORITY, 0, 0);

/* ── Thread 2: WS2812 LED (k_thread_create) ── */
K_THREAD_STACK_DEFINE(ws2812_stack, STACK_SIZE);
static struct k_thread ws2812_thread_data;

void ws2812_thread_fn(void *a, void *b, void *c)
{
	size_t color = 0;
	int rc;

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return;
	}

	LOG_INF("Displaying pattern on strip");
	while (1) {
		for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
			memset(&pixels, 0x00, sizeof(pixels));
			memcpy(&pixels[cursor], &colors[color], sizeof(struct led_rgb));

			rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
			if (rc) {
				LOG_ERR("couldn't update strip: %d", rc);
			}

			k_sleep(DELAY_TIME);
		}

		color = (color + 1) % ARRAY_SIZE(colors);
	}
}

/* ── main: spawn ws2812_thread, hello_thread already running ── */
int main(void)
{
	k_thread_create(&ws2812_thread_data, ws2812_stack,
			K_THREAD_STACK_SIZEOF(ws2812_stack),
			ws2812_thread_fn, NULL, NULL, NULL,
			WS2812_PRIORITY, 0, K_NO_WAIT);

	return 0;
}
