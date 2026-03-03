#include "circular_chart.h"
#include <theme/colors.h>

lv_obj_t *arc;

lv_obj_t * circular_chart_create(lv_obj_t * parent, int value)
{
  lv_obj_t *main_cont = lv_obj_create(parent);
  lv_obj_set_size(main_cont, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_bg_opa(main_cont, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_opa(main_cont, LV_OPA_TRANSP, LV_PART_MAIN);

  // ===== chart ======= //
	static lv_style_t style_label;
	lv_style_init(&style_label);
	lv_style_set_text_color(&style_label, WHITE);

	arc = lv_arc_create(main_cont);

	lv_obj_set_size(arc, 150, 150);
	lv_obj_center(arc);

	lv_arc_set_range(arc, 0, 100);
	lv_arc_set_value(arc, value);

	lv_arc_set_rotation(arc, 90);
	lv_arc_set_bg_angles(arc, 0, 360);

	lv_obj_remove_style(arc, NULL, LV_PART_KNOB);

	lv_obj_set_style_arc_width(arc, 15, LV_PART_MAIN);
	lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);

	lv_obj_set_style_arc_color(arc, GRAY, LV_PART_MAIN);
	lv_obj_set_style_arc_color(arc, LIGHT_BLUE, LV_PART_INDICATOR);

	lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);

	lv_obj_t *label_chart = lv_label_create(arc);
	lv_label_set_text_fmt(label_chart, "%d%%", value);
	lv_obj_center(label_chart);
	lv_obj_add_style(label_chart, &style_label, 0);

  return main_cont;
}

void circular_chart_set_data(lv_obj_t * widget, int value)
{
  if(!widget) return;
  lv_arc_set_value(arc, value);
}
