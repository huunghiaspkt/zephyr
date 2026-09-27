/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: EmbeddedFun <huunghiaspkt@gmail.com>
 */
#include "uart_bridge.h"

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(uart_bridge, LOG_LEVEL_INF);

#define RX_BUF_SIZE   64
#define RX_TIMEOUT_US 2000
#define TX_TIMEOUT_US 10000

static const struct device *host_dev;
static uart_bridge_sink_t sink;

static uint8_t rx_buf[2][RX_BUF_SIZE];
static uint8_t next_buf;

static uint8_t ready_byte = CONFIG_APP_READY_BYTE;

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	ARG_UNUSED(user_data);

	switch (evt->type) {
	case UART_RX_RDY:
		if (sink) {
			sink(&evt->data.rx.buf[evt->data.rx.offset], evt->data.rx.len);
		}
		break;

	case UART_RX_BUF_REQUEST:
		(void)uart_rx_buf_rsp(dev, rx_buf[next_buf], RX_BUF_SIZE);
		next_buf ^= 1;
		break;

	case UART_RX_DISABLED:
		next_buf = 1;
		(void)uart_rx_enable(dev, rx_buf[0], RX_BUF_SIZE, RX_TIMEOUT_US);
		break;

	default:
		break;
	}
}

int uart_bridge_init(uart_bridge_sink_t uart_host)
{
	int rc;

	host_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
	if (!device_is_ready(host_dev)) {
		LOG_ERR("host UART not ready");
		return -ENODEV;
	}
	sink = uart_host;
	rc = uart_callback_set(host_dev, uart_cb, NULL);
	if (rc != 0) {
		LOG_ERR("uart_callback_set failed (%d)", rc);
		return rc;
	}

	next_buf = 1;
	rc = uart_rx_enable(host_dev, rx_buf[0], RX_BUF_SIZE, RX_TIMEOUT_US);
	if (rc != 0) {
		LOG_ERR("uart_rx_enable failed (%d)", rc);
		return rc;
	}

	LOG_INF("host UART up: forwarding received bytes to BLE");
	return 0;
}

int uart_bridge_send_ready(void)
{
	if (host_dev == NULL) {
		return -ENODEV;
	}
	LOG_INF("sending READY (0x%02x) to host", ready_byte);
	return uart_tx(host_dev, &ready_byte, 1, TX_TIMEOUT_US);
}
