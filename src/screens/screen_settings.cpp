#include "screen_settings.h"
#include "screen_location.h"
#include "screen_wifi.h"
#include "screen_dashboard.h"
#include "theme/colors.h"
#include <circular_chart.h>
#include <back_button.h>

static lv_obj_t *screen;

static void event_settings_click(lv_event_t *e)
{
	const char *id = (const char *)lv_event_get_user_data(e);

	if (strcmp(id, "location") == 0)
	{
		screen_location_init();
		lv_screen_load(screen_location_get());
	}
	else if (strcmp(id, "wifi") == 0)
	{
		screen_wifi_init();
		lv_screen_load(screen_wifi_get());
	}
}

void screen_settings_init(void)
{
	screen = lv_obj_create(NULL);

	lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

	static lv_style_t style_H1_label;
	lv_style_init(&style_H1_label);
	lv_style_set_text_color(&style_H1_label, WHITE);
	lv_style_set_text_font(&style_H1_label, &lv_font_montserrat_18);

	/* Title */
	lv_obj_t *title = lv_label_create(screen);
	lv_label_set_text(title, "Settings");
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_add_style(title, &style_H1_label, 0);

	/*Back Button*/
	create_back_button(screen, screen_dashboard_get());

	/* List */
	lv_obj_t *list = lv_list_create(screen);
	lv_obj_set_size(list, LV_PCT(50), LV_PCT(80));
	lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_opa(list, LV_OPA_TRANSP, LV_PART_MAIN);

	/* Menu */
	lv_obj_t *btn_location = lv_list_add_button(list, LV_SYMBOL_GPS, "Location");
	lv_obj_add_event_cb(btn_location, event_settings_click, LV_EVENT_CLICKED, (void *)"location");
	lv_obj_set_style_bg_opa(btn_location, LV_OPA_TRANSP, 0);
	lv_obj_set_style_text_color(btn_location, lv_color_white(), 0);

	lv_obj_t *btn_wifi = lv_list_add_button(list, LV_SYMBOL_WIFI, "WI-FI");
	lv_obj_add_event_cb(btn_wifi, event_settings_click, LV_EVENT_CLICKED, (void *)"wifi");
	lv_obj_set_style_bg_opa(btn_wifi, LV_OPA_TRANSP, 0);
	lv_obj_set_style_text_color(btn_wifi, lv_color_white(), 0);
}

lv_obj_t * screen_settings_get(void)
{
	return screen;
}
