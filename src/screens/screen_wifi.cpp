#include "screen_wifi.h"
#include "screen_dashboard.h"
#include "theme/colors.h"
#include <circular_chart.h>
#include <back_button.h>
#include <screen_settings.h>

static lv_obj_t *screen;

static lv_obj_t *wifi_list;
static lv_obj_t *password_ta;
static char selected_ssid[33];
static lv_timer_t *wifi_timer;
static lv_style_t style_secondary_label;

static lv_style_t style_ta_main;
static lv_style_t style_ta_cursor;

static lv_obj_t *kb;

static lv_obj_t *wifi_popup;
static lv_obj_t *eye_label;

static void ta_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);
  if (code == LV_EVENT_FOCUSED)
  {
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
  }
  else if (code == LV_EVENT_DEFOCUSED)
  {
    lv_keyboard_set_textarea(kb, NULL);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_delete(wifi_popup);
  }
};

static void wifi_screen_event(lv_event_t *e)
{
  if (lv_event_get_code(e) == LV_EVENT_DELETE)
  {
    if (wifi_timer)
    {
      lv_timer_delete(wifi_timer);
      wifi_timer = NULL;
    }
  }
}

static void connect_wifi_event(lv_event_t *e)
{
  const char *pass = lv_textarea_get_text(password_ta);
  Serial.printf("Connecting to %s\n", selected_ssid);
  WiFi.begin(selected_ssid, pass);
  lv_obj_delete(wifi_popup);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

static void cancel_connect_wifi_event(lv_event_t *e)
{
  lv_obj_delete(wifi_popup);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

static void toggle_password(lv_event_t *e)
{
  bool eye_hidden = lv_textarea_get_password_mode(password_ta);
  lv_label_set_text(eye_label, !eye_hidden ? LV_SYMBOL_EYE_CLOSE : LV_SYMBOL_EYE_OPEN);
  lv_textarea_set_password_mode(password_ta, !eye_hidden);
}

static void show_password_dialog(const char *ssid)
{
  strcpy(selected_ssid, ssid);

  wifi_popup = lv_obj_create(screen);
  lv_obj_set_size(wifi_popup, 300, 160);
  lv_obj_align(wifi_popup, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(wifi_popup, COLOR_BG_MODAL_DARK, 0);
  lv_obj_set_style_border_width(wifi_popup, 2, 0);
  lv_obj_set_style_border_color(wifi_popup, MODAL_BORDER, 0);
  lv_obj_clear_flag(wifi_popup, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *label = lv_label_create(wifi_popup);
  lv_label_set_text_fmt(label, "%s", ssid);
  lv_obj_add_style(label, &style_secondary_label, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *row_input = lv_obj_create(wifi_popup);
  lv_obj_set_size(row_input, LV_PCT(100), LV_SIZE_CONTENT);
  lv_obj_align(row_input, LV_ALIGN_CENTER, 0, -5);
  lv_obj_clear_flag(row_input, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_opa(row_input, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_opa(row_input, LV_OPA_TRANSP, 0);
  lv_obj_set_flex_flow(row_input, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row_input,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  /*Show password*/
  lv_obj_t *btn_show = lv_button_create(row_input);
  lv_obj_align(btn_show, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(btn_show, LIGHT_BLUE, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(btn_show, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(btn_show, toggle_password, LV_EVENT_CLICKED, NULL);

  eye_label = lv_label_create(btn_show);
  lv_label_set_text(eye_label, LV_SYMBOL_EYE_CLOSE);
  lv_obj_center(eye_label);

  password_ta = lv_textarea_create(row_input);
  lv_obj_set_size(password_ta, 200, 40);
  lv_textarea_set_password_mode(password_ta, true);
  lv_textarea_set_placeholder_text(password_ta, "Password");

  lv_obj_align(password_ta, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(password_ta, &style_ta_main, LV_PART_MAIN);
  lv_obj_add_style(password_ta, &style_ta_cursor, LV_PART_CURSOR);

  /*Event keyboard*/
  lv_obj_add_event_cb(password_ta, ta_event_cb, LV_EVENT_ALL, kb);
  lv_keyboard_set_textarea(kb, password_ta);

  /*Connect button*/
  lv_obj_t *btn_connect = lv_button_create(wifi_popup);
  lv_obj_align(btn_connect, LV_ALIGN_BOTTOM_RIGHT, -5, 5);
  lv_obj_set_style_bg_color(btn_connect, LIGHT_BLUE, LV_PART_MAIN);
  lv_obj_set_style_border_width(btn_connect, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(btn_connect, LIGHT_BLUE, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(btn_connect, 0, LV_PART_MAIN);

  lv_obj_t *lbl_connect = lv_label_create(btn_connect);
  lv_label_set_text(lbl_connect, "Connect");
  lv_obj_center(lbl_connect);

  lv_obj_add_event_cb(btn_connect, connect_wifi_event, LV_EVENT_CLICKED, NULL);

  /*Cancel button*/
  lv_obj_t *btn_cancel = lv_button_create(wifi_popup);
  lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_LEFT, 5, 5);
  lv_obj_set_style_bg_opa(btn_cancel, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(btn_cancel, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(btn_cancel, LIGHT_BLUE, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(btn_cancel, 0, LV_PART_MAIN);

  lv_obj_t *lbl_cancel = lv_label_create(btn_cancel);
  lv_label_set_text(lbl_cancel, "Cancel");
  lv_obj_center(lbl_cancel);

  lv_obj_add_event_cb(btn_cancel, cancel_connect_wifi_event, LV_EVENT_CLICKED, NULL);
}

static void wifi_item_event(lv_event_t *e)
{
  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
  const char *txt = lv_list_get_button_text(wifi_list, btn);

  show_password_dialog(txt);
  lv_obj_add_state(password_ta, LV_STATE_FOCUSED);
  lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

void wifi_start_scan()
{
  lv_obj_clean(wifi_list);
  lv_obj_t *label = lv_list_add_text(wifi_list, "Scanning...");
  lv_obj_add_style(label, &style_secondary_label, 0);
  lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.scanNetworks(true); // async
}

void wifi_scan_task()
{
  int n = WiFi.scanComplete();

  if (n == -1) // still scanning
    return;
  if (n == -2) // not started
    return;

  lv_obj_clean(wifi_list);

  if (n == 0)
  {
    lv_list_add_text(wifi_list, "No networks");
  }
  else
  {
    for (int i = 0; i < n; i++)
    {
      char buf[64];

      sprintf(
          buf,
          "%s (%d dBm)",
          WiFi.SSID(i).c_str(),
          WiFi.RSSI(i));

      lv_obj_t *btn_item = lv_list_add_button(wifi_list, LV_SYMBOL_WIFI, buf);
      lv_obj_add_event_cb(btn_item, wifi_item_event, LV_EVENT_CLICKED, NULL);
      lv_obj_set_style_bg_opa(btn_item, LV_OPA_TRANSP, 0);
      lv_obj_set_style_text_color(btn_item, WHITE, 0);
    }
  }

  WiFi.scanDelete();
}

static void wifi_timer_cb(lv_timer_t *t)
{
  wifi_scan_task();
}

static void keyboard_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_READY)
  {
  }
}

void screen_wifi_init(void)
{
	screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(screen, COLOR_BG_DARK, 0);
  lv_obj_add_event_cb(screen, wifi_screen_event, LV_EVENT_DELETE, NULL);

  /*Title Style*/
  static lv_style_t style_H1_label;
  lv_style_init(&style_H1_label);
  lv_style_set_text_color(&style_H1_label, WHITE);
  lv_style_set_text_font(&style_H1_label, &lv_font_montserrat_18);

  lv_style_init(&style_secondary_label);
  lv_style_set_text_color(&style_secondary_label, WHITE);
  lv_style_set_text_font(&style_secondary_label, &lv_font_montserrat_14);

  /* ===== Textarea MAIN style ===== */
  lv_style_init(&style_ta_main);
  lv_style_set_text_color(&style_ta_main, WHITE);
  lv_style_set_bg_opa(&style_ta_main, LV_OPA_TRANSP);

  /* ===== Cursor style ===== */
  lv_style_init(&style_ta_cursor);
  lv_style_set_bg_color(&style_ta_cursor, LIGHT_BLUE);
  lv_style_set_bg_opa(&style_ta_cursor, LV_OPA_COVER);
  lv_style_set_width(&style_ta_cursor, 1);

  /* Title */
  lv_obj_t *title = lv_label_create(screen);
  lv_label_set_text(title, "Wi-Fi Settings");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_add_style(title, &style_H1_label, 0);

  /*--- Back Button ---*/
  create_back_button(screen, screen_settings_get());

  wifi_list = lv_list_create(screen);
  lv_obj_set_size(wifi_list, LV_PCT(60), LV_PCT(50));
  lv_obj_set_style_bg_opa(wifi_list, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(wifi_list, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_align(wifi_list, LV_ALIGN_CENTER, 0, 0);

  /* ===== Keyboard ===== */
  kb = lv_keyboard_create(screen);
  lv_obj_set_size(kb, LV_PCT(100), 150);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN); // hidden at first
  lv_obj_add_event_cb(kb, keyboard_event_cb, LV_EVENT_ALL, NULL);

  wifi_start_scan();

  wifi_timer = lv_timer_create(
      wifi_timer_cb,
      500,
      NULL);
}

lv_obj_t * screen_wifi_get(void)
{
  return screen;
}
