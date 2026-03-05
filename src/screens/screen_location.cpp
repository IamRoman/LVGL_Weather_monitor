#include "screen_location.h"
#include "theme/colors.h"
#include <circular_chart.h>
#include <back_button.h>
#include <screen_settings.h>
#include <cstdio>
#include <config_service.h>
#include <loader.h>

extern void update_weather(void);

static lv_obj_t *screen;
static lv_obj_t *anim_cont;
static lv_obj_t *city_container;
static lv_obj_t *ta_lat;
static lv_obj_t *ta_lon;

static lv_obj_t *selected_btn = NULL;
static lv_obj_t *selected_location;

static lv_obj_t *kb;

static lv_style_t style_ta_main;
static lv_style_t style_ta_cursor;

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

static void anim_y_cb(void *var, int32_t v)
{
  lv_obj_set_y((lv_obj_t *)var, v);
};

static void anim_screen(bool isMovingUp)
{
  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, anim_cont);
  if (isMovingUp)
  {
    lv_anim_set_duration(&anim, 100);
    lv_anim_set_values(&anim, lv_obj_get_y(anim_cont), -100);
    lv_anim_set_path_cb(&anim, lv_anim_path_overshoot);
  }
  else
  {
    lv_anim_set_duration(&anim, 100);
    lv_anim_set_values(&anim, lv_obj_get_y(anim_cont), 0);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in);
  }
  lv_anim_set_exec_cb(&anim, anim_y_cb);
  lv_anim_start(&anim);
};

static void ta_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);
  if (code == LV_EVENT_FOCUSED)
  {
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
    anim_screen(true);
  }
  else if (code == LV_EVENT_DEFOCUSED)
  {
    lv_keyboard_set_textarea(kb, NULL);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    anim_screen(false);
  }
};

static void confirm_event_cb(lv_event_t *e)
{
  float lat = atof(lv_textarea_get_text(ta_lat));
  float lon = atof(lv_textarea_get_text(ta_lon));

  config_set_location(lat, lon);

  char buf_long[40];
  sprintf(buf_long, "Selected location: %.4f, %.4f", lat, lon);
  lv_label_set_text(selected_location, buf_long);

  anim_screen(false);

  loader_show(screen);
  lv_refr_now(NULL);
  update_weather();
  loader_hide();
}

static void keyboard_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_READY)
  {
    float lat = atof(lv_textarea_get_text(ta_lat));
    float lon = atof(lv_textarea_get_text(ta_lon));

    config_set_location(lat, lon);

    char buf_long[40];
    sprintf(buf_long, "Selected location: %.4f, %.4f", lat, lon);
    lv_label_set_text(selected_location, buf_long);

    anim_screen(false);

    loader_show(screen);
    lv_refr_now(NULL);
    update_weather();
    loader_hide();
  }
}

void screen_location_init(void)
{
	screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);

  anim_cont = lv_obj_create(screen);
  lv_obj_set_size(anim_cont, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(anim_cont, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(anim_cont, LV_OPA_TRANSP, LV_PART_MAIN);

  /*Title*/
  static lv_style_t style_H1_label;
  lv_style_init(&style_H1_label);
  lv_style_set_text_color(&style_H1_label, WHITE);
  lv_style_set_text_font(&style_H1_label, &lv_font_montserrat_18);

  static lv_style_t style_secondary_label;
  lv_style_init(&style_secondary_label);
  lv_style_set_text_color(&style_secondary_label, WHITE);
  lv_style_set_text_font(&style_secondary_label, &lv_font_montserrat_14);

  /* ===== Keyboard ===== */
  kb = lv_keyboard_create(screen);
  lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER); // only numbers, a dot and a minus
  lv_obj_set_size(kb, LV_PCT(100), 150);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN); // hidden at first
  lv_obj_add_event_cb(kb, keyboard_event_cb, LV_EVENT_ALL, NULL);

  /* Title */
  lv_obj_t *title = lv_label_create(anim_cont);
  lv_label_set_text(title, "Location settings");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_add_style(title, &style_H1_label, 0);

  /* Selected location */
  selected_location = lv_label_create(anim_cont);
  lv_label_set_text(selected_location, "Selected location: --,--");
  lv_obj_align(selected_location, LV_ALIGN_TOP_MID, 0, 45);
  lv_obj_add_style(selected_location, &style_secondary_label, 0);

  /*--- Back Button ---*/
  create_back_button(screen, screen_settings_get());

  /*--- Horizontal list ---*/
  city_container = lv_obj_create(anim_cont);
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

  /* ===== Textarea MAIN style ===== */
  lv_style_init(&style_ta_main);
  lv_style_set_text_color(&style_ta_main, WHITE);
  lv_style_set_bg_opa(&style_ta_main, LV_OPA_TRANSP);

  /* ===== Cursor style ===== */
  lv_style_init(&style_ta_cursor);
  lv_style_set_bg_color(&style_ta_cursor, LIGHT_BLUE);
  lv_style_set_bg_opa(&style_ta_cursor, LV_OPA_COVER);
  lv_style_set_width(&style_ta_cursor, 1);

  /* ===== Label style ===== */
  static lv_style_t style_input_label;
  lv_style_init(&style_input_label);
  lv_style_set_text_color(&style_input_label, WHITE);
  lv_style_set_text_font(&style_input_label, &lv_font_montserrat_14);

  /* ===== Latitude ===== */

  lv_obj_t *row_lat = lv_obj_create(anim_cont);
  lv_obj_set_width(row_lat, LV_PCT(100));
  lv_obj_set_height(row_lat, LV_SIZE_CONTENT);
  lv_obj_align(row_lat, LV_ALIGN_TOP_MID, 0, 130);

  lv_obj_set_style_bg_opa(row_lat, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_opa(row_lat, LV_OPA_TRANSP, 0);

  lv_obj_set_flex_flow(row_lat, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row_lat,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_set_style_pad_gap(row_lat, 10, 0);

  /*Latitude Label*/
  lv_obj_t *lbl_lat = lv_label_create(row_lat);
  lv_label_set_text(lbl_lat, "Lat:   ");
  lv_obj_add_style(lbl_lat, &style_input_label, 0);
  /*Latitude Input*/
  ta_lat = lv_textarea_create(row_lat);
  lv_obj_set_size(ta_lat, 200, 40);
  // lv_obj_align(ta_lat, LV_ALIGN_TOP_MID, 0, 150);
  lv_textarea_set_placeholder_text(ta_lat, "Latitude");

  lv_obj_add_style(ta_lat, &style_ta_main, LV_PART_MAIN);
  lv_obj_add_style(ta_lat, &style_ta_cursor, LV_PART_CURSOR);

  /*Open keyboard*/
  lv_obj_add_event_cb(ta_lat, ta_event_cb, LV_EVENT_ALL, kb);
  lv_keyboard_set_textarea(kb, ta_lat);

  // /* ===== Longitude ===== */
  lv_obj_t *row_lon = lv_obj_create(anim_cont);
  lv_obj_set_width(row_lon, LV_PCT(100));
  lv_obj_set_height(row_lon, LV_SIZE_CONTENT);
  lv_obj_align(row_lon, LV_ALIGN_TOP_MID, 0, 180);

  lv_obj_set_style_bg_opa(row_lon, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_opa(row_lon, LV_OPA_TRANSP, 0);

  lv_obj_set_flex_flow(row_lon, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row_lon,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_set_style_pad_gap(row_lon, 10, 0);
  /*Longitude Label*/
  lv_obj_t *lbl_lon = lv_label_create(row_lon);
  lv_label_set_text(lbl_lon, "Long:");
  lv_obj_add_style(lbl_lon, &style_input_label, 0);
  /*Longitude Input*/
  ta_lon = lv_textarea_create(row_lon);
  lv_obj_set_size(ta_lon, 200, 40);
  // lv_obj_align(ta_lon, LV_ALIGN_TOP_MID, 0, 200);
  lv_textarea_set_placeholder_text(ta_lon, "Longitude");

  lv_obj_add_style(ta_lon, &style_ta_main, LV_PART_MAIN);
  lv_obj_add_style(ta_lon, &style_ta_cursor, LV_PART_CURSOR);

  /*Open keyboard*/
  lv_obj_add_event_cb(ta_lon, ta_event_cb, LV_EVENT_ALL, kb);
  lv_keyboard_set_textarea(kb, ta_lon);

  /* ===== Confirm ===== */
  lv_obj_t *btn_confirm = lv_button_create(screen);
  lv_obj_remove_style_all(btn_confirm);
  lv_obj_set_style_bg_opa(btn_confirm, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_opa(btn_confirm, LV_OPA_TRANSP, 0);

  lv_obj_set_size(btn_confirm, LV_SIZE_CONTENT, 40);

  lv_obj_align(btn_confirm, LV_ALIGN_TOP_RIGHT, -10, 12);

  // Making a horizontal layout
  lv_obj_set_flex_flow(btn_confirm, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_gap(btn_confirm, 5, 0);

  // Btn icon
  lv_obj_t *icon = lv_label_create(btn_confirm);
  lv_label_set_text(icon, LV_SYMBOL_OK);
  lv_obj_set_style_text_color(icon, WHITE, 0);

  // Btn text
  lv_obj_t *label = lv_label_create(btn_confirm);
  lv_label_set_text(label, "Confirm");
  lv_obj_set_style_text_color(label, WHITE, 0);

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
