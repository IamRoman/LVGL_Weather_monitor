#include <lvgl.h>
#include <TFT_eSPI.h>

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  480
#define TFT_ROTATION  LV_DISPLAY_ROTATION_90

TFT_eSPI tft = TFT_eSPI();

int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	uint32_t w = area->x2 - area->x1 + 1;
	uint32_t h = area->y2 - area->y1 + 1;

	// Data transfer to TFT
	tft.startWrite();
	tft.setAddrWindow(area->x1, area->y1, w, h);

	// px_map is simply an array of colors in LV_COLOR_DEPTH format
	tft.pushColors((uint16_t *)px_map, w * h, true);

	tft.endWrite();

	lv_display_flush_ready(disp); // notify LVGL that the flush is complete
}

void display_init(void)
{
   // ==== Create display with TFT_eSPI helper ====
		lv_display_t * disp;
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
		lv_display_set_flush_cb(disp, disp_flush);
		lv_display_set_rotation(disp, TFT_ROTATION);
}
