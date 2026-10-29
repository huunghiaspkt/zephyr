/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define BLINK_MS 250
#define SLOW_MS 2000
#define MY_STACK_SIZE 1024
#define MY_PRIORITY 10

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec bn1 = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec bn2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
static struct gpio_callback bn1_cb;
static struct gpio_callback bn2_cb;

K_THREAD_STACK_DEFINE(my_stack, MY_STACK_SIZE);
static struct k_work_q my_queue;

static void blink_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(blink_work, blink_handler);

static void blink_handler(struct k_work *work)
{
	gpio_pin_toggle_dt(&led);
	k_work_schedule(&blink_work, K_MSEC(BLINK_MS));
}

static void slow_handler(struct k_work *work)
{
	const char *queue = k_thread_name_get(k_current_get());

	printk("slow job starts on %s\n", queue);
	k_busy_wait(SLOW_MS * 1000);
	printk("slow job done on %s\n", queue);
}

static K_WORK_DEFINE(slow_on_system, slow_handler);
static K_WORK_DEFINE(slow_on_mine, slow_handler);

static void bn1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_work_submit(&slow_on_system);
}

static void bn2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_work_submit_to_queue(&my_queue, &slow_on_mine);
}

static void button_init(const struct gpio_dt_spec *bn, struct gpio_callback *cb,
			gpio_callback_handler_t handler)
{
	gpio_pin_configure_dt(bn, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(bn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(cb, handler, BIT(bn->pin));
	gpio_add_callback(bn->port, cb);
}

int main(void)
{
	struct k_work_queue_config cfg = { .name = "my_queue" };

	if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&bn1) || !gpio_is_ready_dt(&bn2)) {
		return 0;
	}
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	button_init(&bn1, &bn1_cb, bn1_pressed);
	button_init(&bn2, &bn2_cb, bn2_pressed);

	k_work_queue_init(&my_queue);
	k_work_queue_start(&my_queue, my_stack, K_THREAD_STACK_SIZEOF(my_stack), MY_PRIORITY, &cfg);

	k_work_schedule(&blink_work, K_NO_WAIT);
	printk("BN1: slow job on the system queue. BN2: slow job on my_queue.\n");
	return 0;
}
