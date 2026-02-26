#include "weather_service.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

#define OWM_HOST "api.openweathermap.org"
#define OWM_PORT 443

#define NOAA_HOST "services.swpc.noaa.gov"
#define NOAA_PORT 443

static const char *API_KEY = "d0e4132a4b555118a94675b394ef386f";
static const char* CITY = "Vinnytsia";
static const char* COUNTRY = "UA";

/* =========================
	 NTP INIT (once)
	 ========================= */
void weather_time_init()
{
	configTzTime("EET-2EEST,M3.5.0/3,M10.5.0/4",
							 "pool.ntp.org",
							 "time.nist.gov");

	struct tm timeinfo;
	int retry = 0;

	while (!getLocalTime(&timeinfo) && retry < 10)
	{
		delay(500);
		retry++;
	}
}

// ==========================
// NOAA 7 DAY FORECAST
// ==========================
void get_NOAA_weather(WeatherData &data)
{
	Serial.println("Connecting to NOAA...");
	WiFiClientSecure forecastClient;
	forecastClient.setInsecure();
	forecastClient.setTimeout(8000);

	if (forecastClient.connect(NOAA_HOST, NOAA_PORT))
	{
		String url = "/products/noaa-planetary-k-index-forecast.json";

		forecastClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
												 "Host: " + NOAA_HOST + "\r\n" +
												 "Connection: close\r\n\r\n");

		String status = forecastClient.readStringUntil('\n');

		if (status.startsWith("HTTP/1.1 200"))
		{
			while (forecastClient.connected())
			{
				String line = forecastClient.readStringUntil('\n');
				if (line == "\r")
					break;
			}

			String payload = forecastClient.readString();

			StaticJsonDocument<12000> doc;
			DeserializationError err = deserializeJson(doc, payload);

			if (!err)
			{
				// reset
				for (int d = 0; d < KP_FORECAST_DAYS; d++)
				{
					data.kp_forecast[d] = 0;
					data.storm_forecast[d] = 0;
				}

				// current date (UTC)
				time_t now = time(nullptr);
				struct tm todayUTC;
				gmtime_r(&now, &todayUTC);
				todayUTC.tm_hour = 0;
				todayUTC.tm_min = 0;
				todayUTC.tm_sec = 0;
				time_t today_start = mktime(&todayUTC); // start of today UTC

				// iterate over all JSON records
				for (int i = 1; i < doc.size(); i++)
				{ // we start with 1, because the 0th line is the header
					String type = doc[i][2].as<const char *>();
					if (type != "observed" && type != "estimated" && type != "predicted")
						continue;

					float kp = doc[i][1].as<float>();

					int year, month, day, hour, min, sec;
					sscanf(doc[i][0], "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);

					struct tm forecast_tm = {};
					forecast_tm.tm_year = year - 1900;
					forecast_tm.tm_mon = month - 1;
					forecast_tm.tm_mday = day;
					forecast_tm.tm_hour = hour;
					forecast_tm.tm_min = min;
					forecast_tm.tm_sec = sec;

					time_t forecast_time = mktime(&forecast_tm);

					int dayDiff = (forecast_time - today_start) / 86400; // difference in days from today

					if (dayDiff < 0 || dayDiff >= KP_FORECAST_DAYS)
						continue; // we miss past or distant days

					if (kp > data.kp_forecast[dayDiff])
						data.kp_forecast[dayDiff] = kp;
				}

				// convert to G-level
				for (int d = 0; d < KP_FORECAST_DAYS; d++)
				{
					float kp = data.kp_forecast[d];
					if (kp >= 9)
						data.storm_forecast[d] = 5;
					else if (kp >= 8)
						data.storm_forecast[d] = 4;
					else if (kp >= 7)
						data.storm_forecast[d] = 3;
					else if (kp >= 6)
						data.storm_forecast[d] = 2;
					else if (kp >= 5)
						data.storm_forecast[d] = 1;
					else
						data.storm_forecast[d] = 0;

					Serial.print("Day ");
					Serial.print(d);
					Serial.print(" max Kp: ");
					Serial.println(data.kp_forecast[d]);
				}
			}
		}

		forecastClient.stop();
	}
}

/* =========================
	 WEATHER UPDATE
	 ========================= */
bool weather_update(WeatherData &data)
{
	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("WiFi NOT connected");
		return false;
	}

	WiFiClientSecure client;
	client.setInsecure();
	client.setTimeout(8000);

	Serial.println("Connecting to OWM...");

	if (!client.connect(OWM_HOST, OWM_PORT))
	{
		Serial.println("Connection FAILED");
		return false;
	}

	Serial.println("Connected!");

	String url = "/data/2.5/forecast?q=";
	url += CITY;
	url += ",";
	url += COUNTRY;
	url += "&appid=";
	url += API_KEY;
	url += "&units=metric";

	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
							 "Host: " + OWM_HOST + "\r\n" +
							 "Connection: close\r\n\r\n");

	// Status
	String status = client.readStringUntil('\n');
	Serial.println(status);

	if (!status.startsWith("HTTP/1.1 200"))
	{
		Serial.println("HTTP ERROR");
		client.stop();
		return false;
	}

	// Skip headers
	while (client.connected())
	{
		String line = client.readStringUntil('\n');
		if (line == "\r")
			break;
	}

	Serial.println("Parsing JSON...");

	StaticJsonDocument<6144> doc;

	DeserializationError error = deserializeJson(doc, client);

	if (error)
	{
		Serial.print("JSON ERROR: ");
		Serial.println(error.c_str());
		client.stop();
		return false;
	}

	// Current temperature (first in the list)
	data.temperature = doc["list"][0]["main"]["temp"] | 0.0;
	data.humidity = doc["list"][0]["main"]["humidity"] | 0;
	data.pressure = doc["list"][0]["main"]["pressure"] | 0;
	data.weather_id = doc["list"][0]["weather"][0]["id"] | 0;

	JsonArray list = doc["list"];

	for (int i = 0; i < FORECAST_SIZE && i < list.size(); i++)
	{
		data.hourly_temp[i] = list[i]["main"]["temp"] | 0.0;

		Serial.print("Forecast ");
		Serial.print(i);
		Serial.print(": ");
		Serial.println(data.hourly_temp[i]);
	}

	client.stop();
	Serial.println("Weather updated OK");
	get_NOAA_weather(data);

	return true;
}
