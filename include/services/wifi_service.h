#pragma once
#include <Arduino.h>

void wifi_init(const char* ssid, const char* password);
void wifi_loop();

bool wifi_is_connected();
int  wifi_get_rssi();
int  wifi_get_signal_percent();
