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
#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <stdnoreturn.h>
#include <Wire.h>

ADS1115_WE adc = ADS1115_WE(0x48);
BME280I2C bmp;
SimpleDHT22 dht22(14);

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
}

void loop(void)
{
        delay(3000);
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
}

_Noreturn void fail(void)
{
	// Perform some random blinknig of the builtin LED to indicate a
	// failure.
	pinMode(LED_BUILTIN, OUTPUT);
	for (unsigned int i = 0; i < 16; i++) {
	        digitalWrite(LED_BUILTIN, HIGH);
		delay(600);
		digitalWrite(LED_BUILTIN, LOW);
		delay(600);
	}
	for (;;) {}
}
