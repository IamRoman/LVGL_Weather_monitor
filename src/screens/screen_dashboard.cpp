#include <WiFi.h>
#include "screen_dashboard.h"
#include "theme/colors.h"
#include <wifi_widget.h>
#include <digital_clock.h>

static lv_obj_t *screen;
static lv_obj_t *wifi_widget;

static void wifi_update_timer(lv_timer_t *timer)
{
	if (WiFi.status() != WL_CONNECTED)
	{
		wifi_icon_set_level(wifi_widget, 0);
		return;
	}

	int rssi = WiFi.RSSI();
	uint8_t level;

	if (rssi > -60)
		level = 4;
	else if (rssi > -70)
		level = 3;
	else if (rssi > -80)
		level = 2;
	else if (rssi > -90)
		level = 1;
	else
		level = 0;

	wifi_icon_set_level(wifi_widget, level);
}

static void clean(lv_obj_t * obj)
{
	lv_obj_set_style_pad_all(obj, 0, 0);
	lv_obj_set_style_pad_row(obj, 0, 0);      // ВАЖЛИВО
	lv_obj_set_style_pad_column(obj, 0, 0);   // ВАЖЛИВО
	lv_obj_set_style_margin_all(obj, 0, 0);
	lv_obj_set_style_border_width(obj, 0, 0);
	lv_obj_set_style_radius(obj, 0, 0);
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static void add_wi_fi_widget(lv_obj_t * obj)
{
  wifi_widget = wifi_icon_create(obj);
	lv_obj_align(wifi_widget, LV_ALIGN_LEFT_MID, 0, 0);
	lv_timer_create(wifi_update_timer, 2000, NULL); // timer 2 seconds (WI-FI Status)
}

void screen_dashboard_init(void)
{
  screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

  /* MAIN */
	lv_obj_t * main = lv_obj_create(screen);
	lv_obj_set_size(main, LV_PCT(100), LV_PCT(100));
	lv_obj_set_layout(main, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(main, LV_FLEX_FLOW_COLUMN);
	clean(main);

	/* ROW 1 */
	lv_obj_t * row1 = lv_obj_create(main);
	lv_obj_set_size(row1, LV_PCT(100), LV_PCT(20));
	lv_obj_set_layout(row1, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(row1, LV_FLEX_FLOW_ROW);
	clean(row1);

	for(int i = 0; i < 3; i++)
	{
			lv_obj_t * w = lv_obj_create(row1);
			clean(w);

			if(i==0) {
				lv_obj_set_size(w, LV_PCT(15), LV_PCT(100));
				lv_obj_set_style_bg_color(w, lv_color_hex(0x3A86FF), 0);
				add_wi_fi_widget(w);
			}
			if(i==1) {
				lv_obj_set_size(w, LV_PCT(70), LV_PCT(100));
				lv_obj_set_style_bg_color(w, lv_color_hex(0x8338EC), 0);
				clock_create(w);
			}
			if(i==2) {
				lv_obj_set_size(w, LV_PCT(15), LV_PCT(100));
				lv_obj_set_style_bg_color(w, lv_color_hex(0xFF006E), 0);
			}
	}

	/* ROW 2 */
	lv_obj_t * row2 = lv_obj_create(main);
	lv_obj_set_size(row2, LV_PCT(100), LV_PCT(80));
	lv_obj_set_layout(row2, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(row2, LV_FLEX_FLOW_ROW);
	clean(row2);

	/* LEFT */
	lv_obj_t * left = lv_obj_create(row2);
	lv_obj_set_size(left, LV_PCT(50), LV_PCT(100));
	clean(left);
	lv_obj_set_style_bg_color(left, lv_color_hex(0x06D6A0), 0);

	/* RIGHT */
	lv_obj_t * right = lv_obj_create(row2);
	lv_obj_set_size(right, LV_PCT(50), LV_PCT(100));
	lv_obj_set_layout(right, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(right, LV_FLEX_FLOW_COLUMN);
	clean(right);

	/* TOP */
	lv_obj_t * top = lv_obj_create(right);
	lv_obj_set_size(top, LV_PCT(100), LV_PCT(50));
	clean(top);
	lv_obj_set_style_bg_color(top, lv_color_hex(0xFFD166), 0);

	/* BOTTOM */
	lv_obj_t * bottom = lv_obj_create(right);
	lv_obj_set_size(bottom, LV_PCT(100), LV_PCT(50));
	clean(bottom);
	lv_obj_set_style_bg_color(bottom, lv_color_hex(0xEF476F), 0);
}

lv_obj_t * screen_dashboard_get(void)
{
    return screen;
}
