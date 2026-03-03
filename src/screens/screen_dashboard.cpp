#include <WiFi.h>
#include "screen_dashboard.h"
#include "theme/colors.h"
#include <wifi_widget.h>
#include <digital_clock.h>
#include <today_weather.h>
#include "kp_chart.h"
#include <settings_button.h>
#include "weather_chart.h"
#include "weather_week.h"

static lv_obj_t *screen;
static lv_obj_t *wifi_widget;
static lv_obj_t *weather_widget;
static lv_obj_t *settings_button;
static kp_chart_t *kp_chart_widget;
static weather_chart_t *weather_chart;
static weather_week_widget *weather_week;

static int32_t x_values[] = {0, 3, 6, 9, 12, 15, 18, 21};
static int week_days = 6;

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

void dashboard_set_weather(const WeatherData &data)
{
	if (weather_widget && kp_chart_widget)
	{
		today_weather_set_data(weather_widget, data);
		kp_chart_set_data(kp_chart_widget, data);
		set_weather_chart_data(weather_chart, x_values, data.hourly_temp, 8);
		weather_week_update(weather_week, data.daily);
	}
}

static void clean(lv_obj_t * obj)
{
	lv_obj_set_style_pad_all(obj, 0, 0);
	lv_obj_set_style_pad_row(obj, 0, 0);
	lv_obj_set_style_pad_column(obj, 0, 0);
	lv_obj_set_style_margin_all(obj, 0, 0);
	lv_obj_set_style_border_width(obj, 0, 0);
	lv_obj_set_style_radius(obj, 0, 0);
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static void add_wi_fi_widget(lv_obj_t * obj)
{
  wifi_widget = wifi_icon_create(obj);
	lv_obj_align(wifi_widget, LV_ALIGN_CENTER, 0, 0);
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
				lv_obj_set_style_bg_color(w, COLOR_BG_DARK, 0);
				add_wi_fi_widget(w);
			}
			if(i==1) {
				lv_obj_set_size(w, LV_PCT(70), LV_PCT(100));
				lv_obj_set_style_bg_color(w, lv_color_hex(0x8338EC), 0);
				lv_obj_set_style_bg_color(w, COLOR_BG_DARK, 0);
				clock_create(w);
			}
			if(i==2) {
				lv_obj_set_size(w, LV_PCT(15), LV_PCT(100));
				lv_obj_set_style_bg_color(w, lv_color_hex(0xFF006E), 0);
				lv_obj_set_style_bg_color(w, COLOR_BG_DARK, 0);
				settings_button = create_settings_icon(w);
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
	lv_obj_set_style_bg_color(left, COLOR_BG_DARK, 0);
	weather_widget = today_weather_create(left);
	lv_obj_set_size(weather_widget,
									LV_PCT(100),
									LV_PCT(40));
	weather_chart = create_weather_chart(left);
	lv_obj_align(weather_chart->chart, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_obj_set_style_border_width(left, 2, LV_PART_MAIN);
	lv_obj_set_style_border_color(left, GRAY, LV_PART_MAIN);
	lv_obj_set_style_border_side(left, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN);

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
	lv_obj_set_style_bg_color(top, COLOR_BG_DARK, 0);
	weather_week = weather_week_create(top, week_days);

	/* BOTTOM */
	lv_obj_t * bottom = lv_obj_create(right);
	lv_obj_set_size(bottom, LV_PCT(100), LV_PCT(50));
	clean(bottom);
	lv_obj_set_style_bg_color(bottom, COLOR_BG_DARK, 0);
	kp_chart_widget = kp_chart_create(bottom);
}

lv_obj_t * screen_dashboard_get(void)
{
    return screen;
}
