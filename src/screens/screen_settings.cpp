#include "screen_settings.h"
#include "screen_home.h"
#include "theme/colors.h"

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

    lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

    static lv_style_t style_label;
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, lv_color_hex(0xfffff));

    // ===== chart ======= //
    int value = 37;
    lv_obj_t *arc = lv_arc_create(screen);

    lv_obj_set_size(arc, 150, 150);
    lv_obj_center(arc);

    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, value); // Value

    lv_arc_set_rotation(arc, 90);
    lv_arc_set_bg_angles(arc, 0, 360);

    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);

    lv_obj_set_style_arc_width(arc, 15, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);

    lv_obj_set_style_arc_color(arc, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x3A86FF), LV_PART_INDICATOR);

    lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);

    lv_obj_t *label_chart = lv_label_create(arc);
    lv_label_set_text_fmt(label_chart, "%d%%", value);
    lv_obj_center(label_chart);
    lv_obj_add_style(label_chart, &style_label, 0);
    // ==================== //
    lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "SETTINGS");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_style(label, &style_label, 0);

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
