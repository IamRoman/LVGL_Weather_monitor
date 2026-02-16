#include "wifi_service.h"
#include <WiFi.h>

static const char* _ssid;
static const char* _pass;
static unsigned long lastReconnectAttempt = 0;

void wifi_init(const char* ssid, const char* password)
{
    _ssid = ssid;
    _pass = password;

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);
}

void wifi_loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = millis();
            WiFi.disconnect();
            WiFi.begin(_ssid, _pass);
        }
    }
}

bool wifi_is_connected()
{
    return WiFi.status() == WL_CONNECTED;
}

int wifi_get_rssi()
{
    if (!wifi_is_connected()) return -100;
    return WiFi.RSSI();
}

/* RSSI → % */
int wifi_get_signal_percent()
{
    int rssi = wifi_get_rssi();

    if (rssi <= -100) return 0;
    if (rssi >= -50)  return 100;

    return 2 * (rssi + 100);
}
