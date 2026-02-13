#include "screen_settings.h"
#include "screen_home.h"

static lv_obj_t * screen;

static void back_event(lv_event_t * e)
{
    lv_scr_load_anim(screen_home_get(),
                     LV_SCR_LOAD_ANIM_MOVE_RIGHT,
                     150, 0, false);
}

void screen_settings_init(void)
{
    screen = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x2f1e1e), 0);

    lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "SETTINGS");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * btn = lv_btn_create(screen);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_obj_add_event_cb(btn, back_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t * l = lv_label_create(btn);
    lv_label_set_text(l, "Back");
    lv_obj_center(l);
}

lv_obj_t * screen_settings_get(void)
{
    return screen;
}
