#include "screen_wifi.h"
#include "screen_dashboard.h"
#include "theme/colors.h"
#include <circular_chart.h>

static lv_obj_t *screen;

static void back_event(lv_event_t *e)
{
	lv_scr_load_anim(screen_dashboard_get(),
									 LV_SCR_LOAD_ANIM_MOVE_RIGHT,
									 0, 0, false);
}

void screen_wifi_init(void)
{
	screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

  lv_obj_t * label = lv_label_create(screen);
  lv_label_set_text(label, "Wi-Fi Settings");
  lv_obj_center(label);

  lv_obj_t * back = lv_button_create(screen);
  lv_obj_align(back, LV_ALIGN_BOTTOM_MID, 0, -20);

  lv_obj_t * back_label = lv_label_create(back);
  lv_label_set_text(back_label, "Back");
  lv_obj_center(back_label);

  lv_obj_add_event_cb(back, back_event, LV_EVENT_CLICKED, NULL);
	
}

lv_obj_t * screen_wifi_get(void)
{
  return screen;
}
