#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sht30_demo, LOG_LEVEL_INF);

/* Step 1: get the device (resolved at compile time from DTS) */
static const struct device *sht30 = DEVICE_DT_GET(DT_NODELABEL(sht30));

int main(void)
{
    struct sensor_value temp, hum;

    if (!device_is_ready(sht30)) {
        LOG_ERR("SHT30 not ready");
        return -ENODEV;
    }

    while (1) {
        /* Step 2: trigger a measurement */
        sensor_sample_fetch(sht30);

        /* Step 3: read the measured values */
        sensor_channel_get(sht30, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        sensor_channel_get(sht30, SENSOR_CHAN_HUMIDITY,     &hum);

        LOG_INF("T: %d.%06d C  RH: %d.%06d %%",
                temp.val1, temp.val2,
                hum.val1, hum.val2);

        k_sleep(K_SECONDS(10));
    }
}
