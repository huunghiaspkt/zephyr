/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: EmbeddedFun <huunghiaspkt@gmail.com>
 *
 */
#ifndef BLE_BRIDGE_H_
#define BLE_BRIDGE_H_

#include <stdint.h>

enum ble_bridge_event {
	BLE_BRIDGE_CONNECTED,
	BLE_BRIDGE_SUBSCRIBED,
	BLE_BRIDGE_DISCONNECTED,
};

typedef void (*ble_bridge_observer_t)(enum ble_bridge_event ev);

int ble_bridge_init(ble_bridge_observer_t observer);

void ble_bridge_send(const uint8_t *data, uint16_t len);

void ble_bridge_disconnect(void);

#endif /* BLE_BRIDGE_H_ */
