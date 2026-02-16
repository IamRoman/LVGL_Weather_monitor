#include "wifi_widget.h"

#define WIFI_BAR_COUNT 4

lv_obj_t* wifi_icon_create(lv_obj_t* parent)
{
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_size(container, 45, 35);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    // lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(container, LV_DIR_NONE);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    for(int i = 0; i < WIFI_BAR_COUNT; i++)
    {
        lv_obj_t* bar = lv_obj_create(container);

        lv_obj_set_width(bar, 6);
        lv_obj_set_height(bar, 6 + i * 5);

        // lv_obj_align(bar, LV_ALIGN_BOTTOM_LEFT, -10, 10);

        lv_obj_align(bar, LV_ALIGN_BOTTOM_LEFT,
                     (i * 8)-10,
                     10);

        lv_obj_set_style_radius(bar, 2, 0);
        lv_obj_set_style_bg_color(bar, lv_color_hex(0x444444), 0);
        lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    }

    return container;
}

void wifi_icon_set_level(lv_obj_t* icon, uint8_t level)
{
    if(level > 4) level = 4;

    for(int i = 0; i < WIFI_BAR_COUNT; i++)
    {
        lv_obj_t* bar = lv_obj_get_child(icon, i);

        if(i < level)
            lv_obj_set_style_bg_color(bar, lv_color_hex(0x00ff88), 0);
        else
            lv_obj_set_style_bg_color(bar, lv_color_hex(0x444444), 0);
    }
}
