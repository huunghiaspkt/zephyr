/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include "counter.h"

LOG_MODULE_DECLARE(app);

void counter_show(int count)
{
	LOG_INF("Count %d", count);
}
