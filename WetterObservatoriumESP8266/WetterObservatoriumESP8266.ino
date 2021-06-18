/* WetterObservatoriumESP8266
   Copyright (C) 2020 Karsten Lehmann <mail@kalehmann.de>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; see the file LICENSE.  If not see
   <http://www.gnu.org/licenses/>.  */

#include<ADS1115_WE.h>
#include <Arduino.h>
#include <BME280I2C.h>
#include "config.hpp"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <SimpleDHT.h>
#include <stdnoreturn.h>
#include <time.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "Crypto.h"

ADS1115_WE adc = ADS1115_WE(0x48);
BME280I2C bmp;
SimpleDHT22 dht22(14);
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiClient client;
unsigned long previousMeasurement = 0;

void sendData(void);
String dateHeader(void);
_Noreturn void fail(void);
#ifdef WITH_SERIAL_OUTPUT
#define DEBUG_SERIAL(val) do{ Serial.print(val); } while( false )
#else
#define DEBUG_SERIAL(val) do{ } while ( false )
#endif
#define LED_BUILTIN 16

void setup(void)
{
	unsigned int bmp_retries = 5;

#ifdef WITH_SERIAL_OUTPUT
	Serial.begin(9600);
#endif
	DEBUG_SERIAL("\nConnecting to WiFi with SSID \"");
	DEBUG_SERIAL(WIFI_SSID);
	DEBUG_SERIAL("\" ");
	// Connect to WiFi
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		DEBUG_SERIAL("*");
	}
	DEBUG_SERIAL(" - Success\n");
	DEBUG_SERIAL("IP address : ");
	DEBUG_SERIAL(WiFi.localIP());

	Wire.begin();

	DEBUG_SERIAL("\nConnecting to BMP280 pressure sensor ");
	while(!bmp.begin()) {
		DEBUG_SERIAL("*");
		delay(1000);
		if (0 == bmp_retries--) {
		        DEBUG_SERIAL("No BMP280 connected\n");
			fail();
		}
	}
	DEBUG_SERIAL("\nConnecting to ADS1115 ADC ");
	if(!adc.init()) {
	        DEBUG_SERIAL("No ADS1115 connected!\n");
		fail();
	}
	DEBUG_SERIAL("\n");
	adc.setVoltageRange_mV(ADS1115_RANGE_4096);
	timeClient.begin();
}

void loop(void)
{
        unsigned long now = millis();
	if (previousMeasurement != 0
	    && now - previousMeasurement < MEASURE_INTERVAL * 1000) {
	        return;
	}
	previousMeasurement = now;
	timeClient.update();
	sendData();
}

String dateHeader(void)
{
        static String day_names[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        time_t rawtime;
	struct tm *info;
	rawtime = timeClient.getEpochTime();
	info = gmtime(&rawtime);
	String daysStr = info->tm_mday < 10
	  ? "0" + String(info->tm_mday)
	  : String(info->tm_mday);
	String monthsStr = info->tm_mon < 9
	  ? "0" + String(info->tm_mon + 1)
	  : String(info->tm_mon + 1);
	String yearsStr = String(info->tm_year + 1900);
	String hoursStr = info->tm_hour < 10
	  ? "0" + String(info->tm_hour)
	  : String(info->tm_hour);
	String minuteStr = info->tm_min < 10
	  ? "0" + String(info->tm_min)
	  : String(info->tm_min);
	String secondStr = info->tm_sec < 10
	  ? "0" + String(info->tm_sec)
	  : String(info->tm_sec);

	return day_names[info->tm_wday] + ", " + daysStr + " " + monthsStr +
	  " " + yearsStr + " " + hoursStr + ":" + minuteStr + ":" + secondStr
	  + " GMT";
}

String postBody(void)
{
        float bmp_temp(NAN), bmp_hum(NAN), bmp_pres(NAN), voltage(0);
	byte dht_hum(0), dht_temp(0);
	BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
	BME280::PresUnit presUnit(BME280::PresUnit_Pa);
	int err = dht22.read(&dht_temp, &dht_hum, NULL);
	if (SimpleDHTErrSuccess != err) {
		DEBUG_SERIAL("Read DHT22 failed, err=");
		DEBUG_SERIAL(SimpleDHTErrCode(err));
		DEBUG_SERIAL(",");
		DEBUG_SERIAL(SimpleDHTErrDuration(err));
	}
	bmp.read(bmp_pres, bmp_temp, bmp_hum, tempUnit, presUnit);
	adc.setCompareChannels(ADS1115_COMP_0_GND);
	adc.startSingleMeasurement();
	while(adc.isBusy()) {}
	voltage = adc.getResult_mV();

	DEBUG_SERIAL("\nDHT22     Temp: ");
	DEBUG_SERIAL(dht_temp);
	DEBUG_SERIAL(" C Humidity ");
	DEBUG_SERIAL(dht_hum);
	DEBUG_SERIAL(" %\n");
	DEBUG_SERIAL("BMP280    Temp: ");
	DEBUG_SERIAL(bmp_temp);
	DEBUG_SERIAL(" C Pressure: ");
	DEBUG_SERIAL(bmp_pres / 1000);
	DEBUG_SERIAL(" kPa\n");
	DEBUG_SERIAL("Sun: ");
	DEBUG_SERIAL(voltage / 3000 * 100);
	DEBUG_SERIAL(" %\n");

	return String(
	        "{\"temperature\": ") + String(dht_temp) +
	        ", \"sun\": " + String((int)(voltage / 30 * 10)) +
	        ", \"pressure\": " + String((int)(bmp_pres * 10)) +
	        "}";
}

void sendData(void)
{
        String path = String("/api/") + LOCATION;
	String body = postBody();
	String date = dateHeader();
	String toSign = String("date: ") + date + "\n" + body;
	String hmac = experimental::crypto::SHA256::hmac(toSign, API_KEY, strlen(API_KEY), 32);

	if ((WiFi.status() == WL_CONNECTED)) {
                http.begin(client, API_HOST, API_PORT, path, false);
		http.addHeader("Content-Type", "application/json");
		http.addHeader("Date", dateHeader());
		http.addHeader("Authorization", "hmac username=\"test\", algorithm=\"sha256\", headers=\"date\", signature=\"" + String(hmac.c_str()) + "\"");
		int httpCode = http.POST(body);
		if (httpCode > 0) {
		        if (httpCode == HTTP_CODE_OK) {
			        DEBUG_SERIAL("Data successfully sended\n");
		        } else {
			        DEBUG_SERIAL("Server error: HTTP code " + String(httpCode));
		        }
		} else {
		        DEBUG_SERIAL("HTTP POST failed: " + http.errorToString(httpCode));
		}
		http.end();
	}
}

_Noreturn void fail(void)
{
	// Perform some random blinknig of the builtin LED to indicate a
	// failure.
	pinMode(LED_BUILTIN, OUTPUT);
	for (unsigned int i = 0; i < 16; i++) {
	  // digitalWrite(LED_BUILTIN, HIGH);
		delay(600);
		digitalWrite(LED_BUILTIN, LOW);
		delay(600);
	}
	for (;;) {}
}
