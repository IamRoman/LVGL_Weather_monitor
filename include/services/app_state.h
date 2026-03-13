#define APP_STATE_H

typedef struct {
  double lat;
  double lon;

  char wifi_ssid[33];
  char wifi_pass[65];
} app_state_t;

extern app_state_t app;
