#include <WiFi.h>
#include "screen_home.h"
#include "screen_settings.h"
#include "screen_forecast.h"
#include "theme/colors.h"
#include "digital_clock.h"
#include "wifi_service.h"
#include "wifi_widget.h"

static lv_obj_t *screen;
static lv_obj_t *wifi_widget;

static void btn_settings_event(lv_event_t * e)
{
    screen_settings_init();
    lv_scr_load_anim(screen_settings_get(),
                     LV_SCR_LOAD_ANIM_MOVE_LEFT,
                     150, 0, false);
}

static void btn_forecast_event(lv_event_t * e)
{
    screen_forecast_init();
    lv_scr_load_anim(screen_forecast_get(),
                     LV_SCR_LOAD_ANIM_MOVE_LEFT,
                     150, 0, false);
}

static void btn_toggle_event(lv_event_t *e)
{
	clock_toggle_format();
}

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

void screen_home_init(void)
{
    screen = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

    clock_create(screen);

    wifi_widget = wifi_icon_create(screen);
    lv_obj_align(wifi_widget, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_timer_create(wifi_update_timer, 2000, NULL); // timer 2 seconds (WI-FI Status)

    lv_obj_t *btn = lv_btn_create(screen);
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, 0, 100);
    lv_obj_add_event_cb(btn, btn_toggle_event,
                        LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_clock_label = lv_label_create(btn);
    lv_label_set_text(btn_clock_label, "Toggle 12/24h");
    lv_obj_center(btn_clock_label);

    LV_IMAGE_DECLARE(sun);
    lv_obj_t *img1 = lv_image_create(screen);
    lv_image_set_src(img1, &sun);
    // lv_obj_set_size(img1, 40, 40);
    lv_obj_align(img1, LV_ALIGN_TOP_LEFT, 10, 10);

    static lv_style_t style_label;
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, lv_color_hex(0xfffff));

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "HOME");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_style(label, &style_label, 0);

    lv_obj_t *btn1 = lv_btn_create(screen);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_event_cb(btn1, btn_settings_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label_settings = lv_label_create(btn1);
    lv_label_set_text(label_settings, LV_SYMBOL_SETTINGS " Settings");

    // lv_obj_t * l1 = lv_label_create(btn1);
    // lv_label_set_text(l1, "Settings");
    // lv_obj_center(l1);

    lv_obj_t * btn2 = lv_btn_create(screen);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_event_cb(btn2, btn_forecast_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t * l2 = lv_label_create(btn2);
    lv_label_set_text(l2, "Forecast");
    lv_obj_center(l2);
}

lv_obj_t * screen_home_get(void)
{
    return screen;
}
