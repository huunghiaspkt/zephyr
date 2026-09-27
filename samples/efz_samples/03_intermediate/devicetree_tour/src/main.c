/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * devicetree_tour — read nodes, properties, and aliases at runtime.
 *
 * The devicetree is fully resolved at *build time*. Every DT_* macro
 * below becomes a constant in the binary; nothing is parsed at runtime.
 * After build, you can see the resolved tree at:
 *   build/zephyr/zephyr.dts
 * and the C-side macros at:
 *   build/zephyr/include/generated/zephyr/devicetree_generated.h
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(dt_tour, LOG_LEVEL_INF);

/* --- Node references -------------------------------------------------
 * Three ways to refer to a node. Pick whichever the source DT exposes.
 */

/* By alias — most portable. Defined in `aliases { ... }` in the board DT. */
#define UART_NODE   DT_CHOSEN(zephyr_console)

/* By label — every node with `label:` gets one. */
#define I2C0_NODE   DT_NODELABEL(i2c0)

/* By path — works for any node, even those without labels. */
#define SOC_NODE    DT_PATH(soc)

int main(void)
{
	LOG_INF("--- devicetree tour ---");

	/* (1) Existence + status. DT_NODE_EXISTS resolves at build time;
	 *     DT_NODE_HAS_STATUS narrows to nodes actually enabled.
	 */
	LOG_INF("chosen zephyr,console exists: %s",
		DT_NODE_EXISTS(UART_NODE) ? "yes" : "no");
	LOG_INF("i2c0 status okay:            %s",
		DT_NODE_HAS_STATUS(I2C0_NODE, okay) ? "yes" : "no");

	/* (2) Node name. */
	LOG_INF("uart console node label:     %s", DT_NODE_FULL_NAME(UART_NODE));

	/* (3) Properties — typed. Each property reader is generated based
	 *     on the binding YAML.
	 */
#if DT_NODE_HAS_PROP(UART_NODE, current_speed)
	LOG_INF("uart baud (current-speed):   %d",
		DT_PROP(UART_NODE, current_speed));
#endif

#if DT_NODE_HAS_PROP(I2C0_NODE, clock_frequency)
	LOG_INF("i2c0 clock-frequency:        %d Hz",
		DT_PROP(I2C0_NODE, clock_frequency));
#endif

	/* (4) reg = <addr size>; — first cell. Useful for memory-mapped peripherals. */
#if DT_NODE_HAS_PROP(I2C0_NODE, reg)
	LOG_INF("i2c0 reg base:               0x%08x",
		(unsigned int)DT_REG_ADDR(I2C0_NODE));
#endif

	/* (5) Walking aliases — every entry in `aliases { foo = &bar; }`
	 *     turns into a DT_ALIAS(foo) macro.
	 */
#if DT_HAS_CHOSEN(zephyr_console)
	LOG_INF("zephyr,console resolves to:  %s",
		DT_NODE_FULL_NAME(DT_CHOSEN(zephyr_console)));
#endif

	/* (6) Iterate children. The board DT has /soc/<peripherals>. */
	LOG_INF("Children of /soc that are enabled:");
#define PRINT_OKAY_CHILD(node_id) \
	IF_ENABLED(DT_NODE_HAS_STATUS(node_id, okay), \
		(LOG_INF("  - %s", DT_NODE_FULL_NAME(node_id));))
	DT_FOREACH_CHILD(SOC_NODE, PRINT_OKAY_CHILD)

	LOG_INF("--- tour done ---");
	LOG_INF("Inspect build/zephyr/zephyr.dts for the full resolved tree.");
	return 0;
}
