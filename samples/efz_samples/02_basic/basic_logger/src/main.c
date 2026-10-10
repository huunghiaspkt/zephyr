/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "counter.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
	LOG_ERR("This is an error");
	LOG_WRN("This is a warning");
	LOG_INF("This is information");
	LOG_DBG("This is a debug message");

	for (int count = 0; ; count++) {
		counter_show(count);
		k_msleep(1000);
	}

	return 0;
}
