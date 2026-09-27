/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: EmbeddedFun <huunghiaspkt@gmail.com>
 */

#ifndef UART_BRIDGE_H_
#define UART_BRIDGE_H_

#include <stdint.h>

typedef void (*uart_bridge_sink_t)(const uint8_t *data, uint16_t len);

int uart_bridge_init(uart_bridge_sink_t sink);

int uart_bridge_send_ready(void);

#endif /* UART_BRIDGE_H_ */
