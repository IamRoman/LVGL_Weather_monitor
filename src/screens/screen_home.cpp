#include "screen_home.h"
#include "screen_settings.h"
#include "screen_forecast.h"

static lv_obj_t * screen;

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

void screen_home_init(void)
{
    screen = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1e1e2f), 0);

    lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "HOME");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * btn1 = lv_btn_create(screen);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_event_cb(btn1, btn_settings_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t * l1 = lv_label_create(btn1);
    lv_label_set_text(l1, "Settings");
    lv_obj_center(l1);

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
