#pragma once

void config_load();
void config_save();

// location
bool config_get_location(float *lat, float *lon);
void config_set_location(float lat, float lon);

// wifi
void config_set_wifi(const char *ssid, const char *pass);
bool config_get_wifi(char *ssid, char *pass);