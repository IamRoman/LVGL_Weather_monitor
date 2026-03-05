#include "loader.h"
#include "theme/colors.h"

static lv_obj_t *loader_bg = NULL;
static lv_obj_t *loader_spinner = NULL;

void loader_show(lv_obj_t *parent)
{
	if(loader_bg != NULL) return; // already shown

	// Semi-transparent background
	loader_bg = lv_obj_create(parent);
	lv_obj_set_size(loader_bg, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_bg_color(loader_bg, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(loader_bg, LV_OPA_50, 0);
	lv_obj_clear_flag(loader_bg, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_center(loader_bg);
	lv_obj_move_foreground(loader_bg);
	lv_obj_set_style_border_width(loader_bg, 0, 0);

	// Block clicks under the loader
	lv_obj_add_flag(loader_bg, LV_OBJ_FLAG_CLICKABLE);

	// Spinner
	loader_spinner = lv_spinner_create(loader_bg);
	lv_obj_set_size(loader_spinner, 60, 60);
	lv_obj_center(loader_spinner);

	// arc thickness
	lv_obj_set_style_border_width(loader_spinner, 4, LV_PART_INDICATOR);
	// color of the arc
	lv_obj_set_style_border_color(loader_spinner, LIGHT_BLUE, LV_PART_INDICATOR);
}

void loader_hide(void)
{
	if(loader_bg == NULL) return;

	lv_obj_del(loader_bg);

	loader_bg = NULL;
	loader_spinner = NULL;
}