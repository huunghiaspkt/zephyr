/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: EmbeddedFun <huunghiaspkt@gmail.com>
 *
 * wakeup_over_uart -- a tiny "wake on UART, bridge to a phone, sleep when idle"
 * demo for the nRF52840.
 *
 * The story it tells:
 *   1. The board sleeps in System OFF, drawing almost nothing.
 *   2. The host sends 0x00 over UART. That byte's start bit pulls the RX line
 *      low, which the chip senses as a wake and boots the firmware.
 *   3. The firmware advertises. A phone app connects and subscribes.
 *   4. The firmware sends a READY byte back to the host ("you may stream now").
 *   5. Every byte the host sends over UART is forwarded to the phone.
 *   6. If the host goes quiet for 5 s (no UART data), the board goes back to
 *      sleep. If no phone connects within 30 s of waking, it also sleeps.
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/pm/device.h>
#include <zephyr/sys/poweroff.h>

#include "ble_bridge.h"
#include "uart_bridge.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);
#define WAKE_PORT DT_NODELABEL(gpio0)
#define WAKE_PIN  8
static const struct device *const wake_port = DEVICE_DT_GET(WAKE_PORT);

enum event {
	EV_SUBSCRIBED, 
	EV_DISCONNECTED,
	EV_NO_PHONE,
	EV_UART_IDLE,
};
K_MSGQ_DEFINE(events, sizeof(uint8_t), 8, 1);
static atomic_t forwarding;

static void post(enum event ev)
{
	uint8_t e = (uint8_t)ev;

	(void)k_msgq_put(&events, &e, K_NO_WAIT);
}

static void no_phone_expiry(struct k_timer *t)  { ARG_UNUSED(t); post(EV_NO_PHONE); }
static void uart_idle_expiry(struct k_timer *t) { ARG_UNUSED(t); post(EV_UART_IDLE); }
K_TIMER_DEFINE(no_phone_timer, no_phone_expiry, NULL);
K_TIMER_DEFINE(uart_idle_timer, uart_idle_expiry, NULL);

static inline void uart_idle_restart(void)
{
	k_timer_start(&uart_idle_timer, K_MSEC(CONFIG_APP_UART_IDLE_MS), K_NO_WAIT);
}

static void on_uart_bytes(const uint8_t *data, uint16_t len)
{
	if (atomic_get(&forwarding)) {
		ble_bridge_send(data, len);
		uart_idle_restart();
	}
}

static void on_ble_event(enum ble_bridge_event ev)
{
	switch (ev) {
	case BLE_BRIDGE_CONNECTED:
		k_timer_stop(&no_phone_timer);
		break;
	case BLE_BRIDGE_SUBSCRIBED:
		post(EV_SUBSCRIBED);
		break;
	case BLE_BRIDGE_DISCONNECTED:
		post(EV_DISCONNECTED);
		break;
	}
}

static void go_to_sleep(void)
{
	const struct device *host = DEVICE_DT_GET(DT_NODELABEL(uart0));

	LOG_INF("going to System OFF -- send 0x00 over UART to wake");

	atomic_clear(&forwarding);
	k_timer_stop(&uart_idle_timer);

	(void)pm_device_action_run(host, PM_DEVICE_ACTION_SUSPEND);
	(void)gpio_pin_configure(wake_port, WAKE_PIN, GPIO_INPUT | GPIO_PULL_UP);
	(void)gpio_pin_interrupt_configure(wake_port, WAKE_PIN, GPIO_INT_LEVEL_LOW);

	sys_poweroff();
}

static void supervisor(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	for (;;) {
		uint8_t ev;

		k_msgq_get(&events, &ev, K_FOREVER);

		switch (ev) {
		case EV_SUBSCRIBED:
			LOG_INF("phone ready -> forwarding UART to BLE");
			(void)uart_bridge_send_ready();
			atomic_set(&forwarding, 1);
			uart_idle_restart();
			break;

		case EV_UART_IDLE:
			LOG_INF("no UART data for %d ms", CONFIG_APP_UART_IDLE_MS);
			atomic_clear(&forwarding);
			ble_bridge_disconnect();
			break;

		case EV_NO_PHONE:
			LOG_INF("no phone connected within %d ms", CONFIG_APP_ADV_TIMEOUT_MS);
			go_to_sleep();
			break;

		case EV_DISCONNECTED:
			go_to_sleep();
			break;
		}
	}
}

K_THREAD_DEFINE(supervisor_tid, 2048, supervisor, NULL, NULL, NULL, 6, 0, 0);

int main(void)
{

	LOG_INF("Example: wakeup over UART");

	if (!device_is_ready(wake_port)) {
		LOG_ERR("wake GPIO port not ready");
	}

	if (ble_bridge_init(on_ble_event) != 0) {
		LOG_ERR("BLE init failed");
	}
	if (uart_bridge_init(on_uart_bytes) != 0) {
		LOG_ERR("UART init failed");
	}

	k_timer_start(&no_phone_timer, K_MSEC(CONFIG_APP_ADV_TIMEOUT_MS), K_NO_WAIT);

	return 0;
}
