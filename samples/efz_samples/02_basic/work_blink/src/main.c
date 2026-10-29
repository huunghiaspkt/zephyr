/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define BLINK_MS 500

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static void blink_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(blink_work, blink_handler);

static void blink_handler(struct k_work *work)
{
	gpio_pin_toggle_dt(&led);
	printk("LED1 toggled\n");
	k_work_schedule(&blink_work, K_MSEC(BLINK_MS));
}

int main(void)
{
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

	k_work_schedule(&blink_work, K_NO_WAIT);
	return 0;
}
