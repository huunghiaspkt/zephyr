#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

#include "calc.h"

/* Shell command: `add <a> <b>` -> prints a + b.
 * The pytest harness sends this command and checks the printed result.
 */
static int cmd_add(const struct shell *sh, size_t argc, char **argv)
{
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);

	shell_print(sh, "%d", add(a, b));
	return 0;
}

/* mandatory args = 3 (the command itself + two operands), optional = 0 */
SHELL_CMD_ARG_REGISTER(add, NULL, "Add two integers: add <a> <b>", cmd_add, 3, 0);

int main(void)
{
	return 0;
}
