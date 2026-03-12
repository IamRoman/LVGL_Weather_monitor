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
}

static void show_password_dialog(const char *ssid)
{
  strcpy(selected_ssid, ssid);

  lv_obj_t *modal = lv_obj_create(screen);
  lv_obj_set_size(modal, 260, 160);
  lv_obj_align(modal, LV_ALIGN_TOP_MID, 0, 0);
  // lv_obj_center(modal);
  lv_obj_set_style_bg_color(modal, COLOR_BG_DARK, 0);

  lv_obj_t *label = lv_label_create(modal);
  lv_label_set_text_fmt(label, "%s", ssid);
  lv_obj_add_style(label, &style_secondary_label, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

  password_ta = lv_textarea_create(modal);
  lv_obj_set_size(password_ta, 220, 40);
  lv_textarea_set_password_mode(password_ta, true);
  lv_textarea_set_placeholder_text(password_ta, "Password");

  lv_obj_align(password_ta, LV_ALIGN_CENTER, 0, -5);
  lv_obj_add_style(password_ta, &style_ta_main, LV_PART_MAIN);
  lv_obj_add_style(password_ta, &style_ta_cursor, LV_PART_CURSOR);

  lv_obj_t *btn = lv_button_create(modal);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);

  lv_obj_t *lbl = lv_label_create(btn);
  lv_label_set_text(lbl, "Connect");
  lv_obj_center(lbl);

  lv_obj_add_event_cb(btn, connect_wifi_event, LV_EVENT_CLICKED, NULL);
}

static void wifi_item_event(lv_event_t *e)
{
  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
  const char *txt = lv_list_get_button_text(wifi_list, btn);

  show_password_dialog(txt);
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
