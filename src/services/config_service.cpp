#include <Preferences.h>
#include "app_state.h"

static Preferences prefs;

void config_load()
{
  prefs.begin("weather", true);

  app.lat = prefs.getDouble("lat", 50.45466); // Kyiv by default
  app.lon = prefs.getDouble("lon", 30.5238);

  prefs.end();
}

void config_save()
{
  prefs.begin("weather", false);

  prefs.putDouble("lat", app.lat);
  prefs.putDouble("lon", app.lon);

  prefs.end();
}

void config_set_location(float lat, float lon)
{
  Serial.println("=== CONFIG SET LOCATION ===");
  Serial.printf("Saving lat: %.6f\n", lat);
  Serial.printf("Saving lon: %.6f\n", lon);

  app.lat = lat;
  app.lon = lon;

  config_save();
}

bool config_get_location(float *lat, float *lon)
{
  *lat = app.lat;
  *lon = app.lon;

  Serial.printf("Loaded from RAM lat: %.6f\n", *lat);
  Serial.printf("Loaded from RAM lon: %.6f\n", *lon);

  return true;
}