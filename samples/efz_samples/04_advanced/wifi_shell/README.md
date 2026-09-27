# WiFi + ping over the Zephyr shell

Brings up the Zephyr shell with the WiFi and networking command modules on
an ESP32-S3 DevKitC. Every command (`wifi`, `net ping`, …) comes from the
shell modules enabled in `prj.conf` — `main()` just prints a hint.

Based on `zephyr/samples/net/wifi/shell`.

## Build & flash

Run from this directory (`04_advanced/wifi_shell`):

```console
# Pristine build for the ESP32-S3 DevKitC
west build -p always -b esp32s3_devkitc/esp32s3/procpu .

# Flash and open the serial console
west flash
west espressif monitor      # or: minicom / screen on the UART
```

Build only (no hardware attached):

```console
west build -p always -b esp32s3_devkitc/esp32s3/procpu .
```

## Using it

Built-in commands (free with `CONFIG_NET_L2_WIFI_SHELL` + `CONFIG_NET_SHELL`):

```console
uart:~$ wifi scan
uart:~$ wifi connect -s "MyNetwork" -p MyPassword -k 1   # -k 1 = WPA2-PSK
uart:~$ wifi status
uart:~$ net iface
uart:~$ net ping 8.8.8.8
```

`wifi connect` takes flags: `-s` SSID, `-p` passphrase, `-k` key-mgmt
(0:open, 1:WPA2-PSK, 4:WPA3-SAE-H2E, 9:WPA-PSK; `wifi connect -h` lists all).

Once associated, `CONFIG_ESP32_WIFI_STA_AUTO_DHCPV4` grabs a DHCP lease, so
`net ping <ip>` works straight away.
