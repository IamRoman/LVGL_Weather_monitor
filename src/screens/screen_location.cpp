#include "screen_location.h"
#include "theme/colors.h"
#include <circular_chart.h>
#include <back_button.h>
#include <screen_settings.h>
#include <cstdio>
#include <config_service.h>

static lv_obj_t *screen;
static lv_obj_t *city_container;
static lv_obj_t *ta_lat;
static lv_obj_t *ta_lon;

static lv_obj_t *selected_btn = NULL;
static lv_obj_t *selected_location;

static void city_select(lv_obj_t *btn)
{
  if (selected_btn)
  {
    lv_obj_set_style_bg_opa(selected_btn, LV_OPA_TRANSP, 0);
  }

  selected_btn = btn;

  lv_obj_set_style_bg_color(btn, LIGHT_BLUE, 0);
  lv_obj_set_style_bg_opa(btn, LV_OPA_30, 0);
}

static void city_event_cb(lv_event_t *e)
{
  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
  uint32_t index = (uint32_t)lv_event_get_user_data(e);

  city_select(btn);

  char buf[16];

  sprintf(buf, "%.4f", cities[index].lat);
  lv_textarea_set_text(ta_lat, buf);

  sprintf(buf, "%.4f", cities[index].lon);
  lv_textarea_set_text(ta_lon, buf);
}

static void confirm_event_cb(lv_event_t *e)
{
  float lat = atof(lv_textarea_get_text(ta_lat));
  float lon = atof(lv_textarea_get_text(ta_lon));

  config_set_location(lat, lon);

  char buf_long[40];
  sprintf(buf_long, "Selected location: %.4f, %.4f", lat, lon);
  lv_label_set_text(selected_location, buf_long);
}

void screen_location_init(void)
{
	screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

  /*Title*/
  static lv_style_t style_H1_label;
  lv_style_init(&style_H1_label);
  lv_style_set_text_color(&style_H1_label, WHITE);
  lv_style_set_text_font(&style_H1_label, &lv_font_montserrat_18);

  static lv_style_t style_secondary_label;
  lv_style_init(&style_secondary_label);
  lv_style_set_text_color(&style_secondary_label, WHITE);
  lv_style_set_text_font(&style_secondary_label, &lv_font_montserrat_14);

  /* Title */
  lv_obj_t *title = lv_label_create(screen);
  lv_label_set_text(title, "Location settings");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_add_style(title, &style_H1_label, 0);

  /* Selected location */
  selected_location = lv_label_create(screen);
  lv_label_set_text(selected_location, "Selected location: --,--");
  lv_obj_align(selected_location, LV_ALIGN_TOP_MID, 0, 35);
  lv_obj_add_style(selected_location, &style_secondary_label, 0);

  /*--- Back Button ---*/
  create_back_button(screen, screen_settings_get());

  /*--- Horizontal list ---*/
  city_container = lv_obj_create(screen);
  lv_obj_set_width(city_container, LV_PCT(100));
  lv_obj_set_height(city_container, 60);
  lv_obj_align(city_container, LV_ALIGN_TOP_MID, 0, 60);
  lv_obj_set_style_bg_opa(city_container, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(city_container, LV_OPA_TRANSP, LV_PART_MAIN);

  lv_obj_set_flex_flow(city_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_scroll_dir(city_container, LV_DIR_HOR);
  lv_obj_set_style_pad_gap(city_container, 10, 0);
  lv_obj_set_flex_align(city_container,
                        LV_FLEX_ALIGN_START,   // horizontally
                        LV_FLEX_ALIGN_CENTER,  // vertically
                        LV_FLEX_ALIGN_CENTER); // for multi-line

  for (uint32_t i = 0; i < CITY_COUNT; i++)
  {
    lv_obj_t *item_btn = lv_button_create(city_container);
    lv_obj_set_size(item_btn, 120, 60);
    lv_obj_remove_style_all(item_btn);

    // lv_obj_set_style_bg_opa(item_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(item_btn, 1, 0);
    lv_obj_set_style_border_color(item_btn, LIGHT_BLUE, 0);
    lv_obj_set_style_radius(item_btn, 5, 0);

    lv_obj_t *label = lv_label_create(item_btn);
    lv_label_set_text(label, cities[i].name);
    lv_obj_set_style_pad_all(label, 5, 0);
    lv_obj_add_style(label, &style_secondary_label, 0);
    lv_obj_center(label);

    lv_obj_add_event_cb(item_btn, city_event_cb, LV_EVENT_CLICKED, (void *)i);

    if (i == 0) // Kyiv default
    {
      city_select(item_btn);
    }
  }

  /* ===== Latitude ===== */

  ta_lat = lv_textarea_create(screen);
  lv_obj_set_size(ta_lat, 200, 40);
  lv_obj_align(ta_lat, LV_ALIGN_TOP_MID, 0, 150);
  lv_textarea_set_placeholder_text(ta_lat, "Latitude");
  lv_obj_set_style_text_color(ta_lat, WHITE, 0);
  lv_obj_set_style_bg_opa(ta_lat, LV_OPA_TRANSP, LV_PART_MAIN);

  // /* ===== Longitude ===== */

  ta_lon = lv_textarea_create(screen);
  lv_obj_set_size(ta_lon, 200, 40);
  lv_obj_align(ta_lon, LV_ALIGN_TOP_MID, 0, 200);
  lv_textarea_set_placeholder_text(ta_lon, "Longitude");
  lv_obj_set_style_text_color(ta_lon, WHITE, 0);
  lv_obj_set_style_bg_opa(ta_lon, LV_OPA_TRANSP, LV_PART_MAIN);

  /* ===== Confirm ===== */

  lv_obj_t *btn_confirm = lv_button_create(screen);
  lv_obj_align(btn_confirm, LV_ALIGN_BOTTOM_MID, 0, -20);

  lv_obj_t *lbl = lv_label_create(btn_confirm);
  lv_label_set_text(lbl, "Confirm");
  lv_obj_center(lbl);

  lv_obj_add_event_cb(btn_confirm, confirm_event_cb, LV_EVENT_CLICKED, NULL);

  /* ===== Initial value ===== */

  float lat, lon;

  if (config_get_location(&lat, &lon)) // if available in Preferences
  {
    char buf[40];

    sprintf(buf, "%.4f", lat);
    lv_textarea_set_text(ta_lat, buf);

    sprintf(buf, "%.4f", lon);
    lv_textarea_set_text(ta_lon, buf);

    char buf_long[40];
    sprintf(buf_long, "Selected location: %.4f, %.4f", lat, lon);
    lv_label_set_text(selected_location, buf_long);
  }
}

lv_obj_t * screen_location_get(void)
{
  return screen;
}
