#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
// https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable
#include <dht.h>
#include <Wire.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bme;
dht DHT;

#define DHT22_PIN 14

unsigned long delayTime;

void
setup ()
{
  Serial.begin (9600);
  Serial.println (F ("BMP280 and DHT22 test"));

  bool status;
  status = bme.begin (0x76);
  if (!status)
    {
      Serial.println ("Could not find a valid BMP280 sensor, check wiring!");
      while (1);
    }

  Serial.println ("-- Default Test --");
  delayTime = 1000;

  Serial.println ();
}

void
loop ()
{
  printValues ();
  delay (delayTime);
}

void
printValues ()
{
  int chk = DHT.read22 (DHT22_PIN);
  switch (chk)
    {
    case DHTLIB_OK:
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print ("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print ("Time out error,\t");
      break;
    default:
      Serial.print ("Unknown error,\t");
      break;
    }

  Serial.print ("Temperature (BMP280) = ");
  Serial.print (bme.readTemperature ());
  Serial.print (" *C");
  Serial.print (" (DHT22) = ");
  Serial.print (DHT.temperature, 1);
  Serial.println (" *C");

  Serial.print ("Pressure (BMP280) = ");
  Serial.print (bme.readPressure () / 100.0F);
  Serial.println (" hPa");

  Serial.print ("Humidity (DHT22) = ");
  Serial.print (DHT.humidity, 1);
  Serial.println (" %");

  Serial.print ("Approx. Altitude (BMP280) = ");
  Serial.print (bme.readAltitude (SEALEVELPRESSURE_HPA));
  Serial.println (" m");

  float lightVoltage = analogRead (A0) * 3.3 / 1024;
  Serial.print ("Photoresitor voltage = ");
  Serial.print (lightVoltage);
  Serial.println (" V");

  Serial.println ();
}
