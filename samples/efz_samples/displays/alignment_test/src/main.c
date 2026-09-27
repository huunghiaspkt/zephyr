/* TFT alignment test: black screen, 1-px white border, coloured corners.
 * Expected: red top-left, green top-right, blue bottom-right, grey bottom-left.
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(tft_diag, LOG_LEVEL_INF);

#define W 172
#define H 320
#define C 40	/* corner square size */

static const struct gpio_dt_spec bl = GPIO_DT_SPEC_GET(DT_NODELABEL(tft_bl), gpios);
static const struct device *disp = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static uint16_t line[W];

static uint16_t pixel(int x, int y)
{
	if (x == 0 || y == 0 || x == W - 1 || y == H - 1) {
		return 0xFFFF;					/* white border */
	}
	if (y < C && x < C)                 return 0xF800;	/* red,   top-left */
	if (y < C && x >= W - C)            return 0x07E0;	/* green, top-right */
	if (y >= H - C && x >= W - C)       return 0x001F;	/* blue,  bottom-right */
	if (y >= H - C && x < C)            return 0x8410;	/* grey,  bottom-left */
	return 0x0000;
}

int main(void)
{
	struct display_buffer_descriptor d = {
		.buf_size = sizeof(line), .width = W, .height = 1, .pitch = W,
	};
	int err = 0;

	gpio_pin_configure_dt(&bl, GPIO_OUTPUT_ACTIVE);
	display_blanking_off(disp);
	for (int y = 0; y < H && !err; y++) {
		for (int x = 0; x < W; x++) {
			line[x] = sys_cpu_to_be16(pixel(x, y));
		}
		err = display_write(disp, 0, y, &d, line);
	}
	LOG_INF("alignment pattern drawn: %d", err);
	return 0;
}
