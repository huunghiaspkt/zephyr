/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell commands: WiFi + ping
 *
 * Try:  wifi scan | wifi connect -s "<ssid>" -p <psk> -k 1 | net ping <ip>
 */

#include <stdio.h>

#include <zephyr/kernel.h>

int main(void)
{
    printf("Hello EmbeddedFun!\n");
	printf("Zephyr shell ready.\n");
	printf("Try:  wifi scan  |  wifi connect -s <ssid> -p <psk> -k 1  |  net ping 8.8.8.8\n");
	return 0;
}
