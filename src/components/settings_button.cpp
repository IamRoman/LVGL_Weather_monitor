#include <settings_button.h>
#include "screen_settings.h"
#include "theme/colors.h"

static void btn_settings_event(lv_event_t * e)
{
	screen_settings_init();
	lv_scr_load_anim(screen_settings_get(),
									 LV_SCR_LOAD_ANIM_MOVE_LEFT,
									 0, 0, false);
}

lv_obj_t * create_settings_icon(lv_obj_t * parent)
{
	/* --- Icon --- */
	lv_obj_t * icon = lv_label_create(parent);
	lv_label_set_text(icon, LV_SYMBOL_SETTINGS);

	lv_obj_align(icon, LV_ALIGN_TOP_RIGHT, -10, 10);

	/* Increasing the touch area */
	lv_obj_set_style_pad_all(icon, 15, LV_PART_MAIN);

	/* Making it clickable */
	lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);

	/* Event */
	lv_obj_add_event_cb(icon, btn_settings_event, LV_EVENT_CLICKED, NULL);
	/* Style */
	lv_obj_set_style_text_font(icon, &lv_font_montserrat_22, 0);
	lv_obj_center(icon);
	lv_obj_set_style_text_color(icon, WHITE, LV_PART_MAIN);

	return icon;
}