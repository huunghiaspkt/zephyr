/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * kconfig_demo — shows how Kconfig values become C constants.
 *
 * Try each of these and watch the output change:
 *
 *   # Default greeting from prj.conf, overridden by boards/<board>.conf
 *   west build -b esp32s3_devkitc_procpu .
 *
 *   # Override on the command line — wins over both files.
 *   west build -b esp32s3_devkitc_procpu . -- \
 *       -DCONFIG_DEMO_GREETING='"Hello from CLI"' \
 *       -DCONFIG_DEMO_USE_FANCY_OUTPUT=n
 *
 *   # Inspect the final, merged Kconfig:
 *   cat build/zephyr/.config | grep DEMO_
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(kconfig_demo, LOG_LEVEL_INF);

static void print_fancy(void)
{
	LOG_INF("==========================================");
	LOG_INF("  %s", CONFIG_DEMO_GREETING);
	LOG_INF("  loop period = %d ms", CONFIG_DEMO_LOOP_PERIOD_MS);
	LOG_INF("==========================================");
}

static void print_plain(void)
{
	LOG_INF("%s (period=%d ms)",
		CONFIG_DEMO_GREETING, CONFIG_DEMO_LOOP_PERIOD_MS);
}

int main(void)
{
	/* IS_ENABLED() is the canonical way to check a bool CONFIG_*.
	 * Both branches always compile — the dead one is dropped by the
	 * optimizer. Prefer this to #ifdef.
	 */
	if (IS_ENABLED(CONFIG_DEMO_USE_FANCY_OUTPUT)) {
		print_fancy();
	} else {
		print_plain();
	}

	while (1) {
		k_sleep(K_MSEC(CONFIG_DEMO_LOOP_PERIOD_MS));
		LOG_INF("tick");
	}
}
