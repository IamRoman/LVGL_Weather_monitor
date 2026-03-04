#pragma once

void config_load();
void config_save();

bool config_get_location(float *lat, float *lon);
void config_set_location(float lat, float lon);