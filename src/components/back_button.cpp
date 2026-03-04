#include <back_button.h>
#include "theme/colors.h"

static void back_event_cb(lv_event_t * e)
{
  lv_obj_t * target_screen = (lv_obj_t *)lv_event_get_user_data(e);
  lv_screen_load(target_screen);
}

lv_obj_t * create_back_button(lv_obj_t * parent, lv_obj_t * target_screen)
{
  lv_obj_t * btn = lv_button_create(parent);

  /* We remove all the standard style */
  lv_obj_remove_style_all(btn);

  lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 10, 0);

  /* We make the size according to the content */
  lv_obj_set_size(btn, LV_SIZE_CONTENT, 40);

  lv_obj_t * label = lv_label_create(btn);
  lv_label_set_text(label, LV_SYMBOL_LEFT " Go Back");
  lv_obj_set_style_text_color(label, lv_color_white(), 0);
  lv_obj_center(label);

  lv_obj_add_event_cb(btn, back_event_cb, LV_EVENT_CLICKED, target_screen);

  return btn;
}