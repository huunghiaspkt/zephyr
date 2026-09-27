/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: EmbeddedFun <huunghiaspkt@gmail.com>
 */
#include "ble_bridge.h"

#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci_types.h>

#include <bluetooth/services/nus.h>

LOG_MODULE_REGISTER(ble_bridge, LOG_LEVEL_INF);

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define ATT_NOTIFY_OVERHEAD 3
#define MAX_CHUNK 244

struct tx_chunk {
	uint16_t len;
	uint8_t data[MAX_CHUNK];
};
K_MSGQ_DEFINE(tx_msgq, sizeof(struct tx_chunk), 16, 4);

static struct bt_conn *current_conn;
static atomic_t notify_enabled;
static ble_bridge_observer_t observer;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

static void advertising_start(void)
{
	int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, ad, ARRAY_SIZE(ad),
				  sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("advertising start failed (%d)", err);
		return;
	}
	LOG_INF("advertising as \"%s\"", DEVICE_NAME);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_ERR("connection failed (0x%02x)", err);
		return;
	}
	current_conn = bt_conn_ref(conn);
	LOG_INF("phone connected");
	if (observer) {
		observer(BLE_BRIDGE_CONNECTED);
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(conn);
	LOG_INF("phone disconnected (0x%02x)", reason);
	atomic_clear(&notify_enabled);
	if (current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}
	if (observer) {
		observer(BLE_BRIDGE_DISCONNECTED);
	}
}

static void nus_send_enabled(enum bt_nus_send_status status)
{
	bool on = (status == BT_NUS_SEND_STATUS_ENABLED);

	atomic_set(&notify_enabled, on ? 1 : 0);
	LOG_INF("notifications %s", on ? "enabled" : "disabled");
	if (on && observer) {
		observer(BLE_BRIDGE_SUBSCRIBED);
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static struct bt_nus_cb nus_cb = {
	.send_enabled = nus_send_enabled,
};

void ble_bridge_send(const uint8_t *data, uint16_t len)
{
	while (len > 0) {
		struct tx_chunk chunk;

		chunk.len = MIN(len, (uint16_t)MAX_CHUNK);
		memcpy(chunk.data, data, chunk.len);

		if (k_msgq_put(&tx_msgq, &chunk, K_NO_WAIT) != 0) {
			LOG_WRN("BLE TX queue full; dropping %u B", chunk.len);
			return;
		}
		data += chunk.len;
		len -= chunk.len;
	}
}

void ble_bridge_disconnect(void)
{
	if (current_conn) {
		(void)bt_conn_disconnect(current_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
	}
}

static void tx_thread(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (;;) {
		struct tx_chunk chunk;

		k_msgq_get(&tx_msgq, &chunk, K_FOREVER);

		if (current_conn == NULL || !atomic_get(&notify_enabled)) {
			continue;
		}

		int retries = 10;
		int err;

		do {
			err = bt_nus_send(current_conn, chunk.data, chunk.len);
			if (err == -ENOMEM) {
				k_msleep(2);
			}
		} while (err == -ENOMEM && --retries > 0);

		if (err) {
			LOG_WRN("nus_send %u B failed (%d)", chunk.len, err);
		}
	}
}

K_THREAD_DEFINE(nus_tx_tid, 1024, tx_thread, NULL, NULL, NULL, 7, 0, 0);

int ble_bridge_init(ble_bridge_observer_t obs)
{
	int err;

	observer = obs;

	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("bt_enable failed (%d)", err);
		return err;
	}

	err = bt_nus_init(&nus_cb);
	if (err) {
		LOG_ERR("bt_nus_init failed (%d)", err);
		return err;
	}

	advertising_start();
	return 0;
}
