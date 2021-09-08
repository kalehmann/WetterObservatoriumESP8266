## WetterObservatoriumESP8266

This repository contains the code for an ESP8266 to communicate with the
[WeatherObservatorium][git_wo].

Currently the code supports the following sensors:
* DHT 22 temperature and humidity sensor
* BMP 280 temperature and pressure sensor
* ADS1115 with photo resistor on the first port

### Configuration

The configuration happens in the file [`config.hpp`][config_header].
The following constants can be set:

| Constant           | Description                                                         |
|--------------------|---------------------------------------------------------------------|
| `WIFI_SSID`        | The SSID of the WiFi network                                        |
| `WIFI_PASS`        | The password of the WiFi network                                    |
| `API_HOST`         | The address of the WeatherObservatorium (without protocol and port) |
| `API_PORT`         | The port of the WeatherObservatorium                                |
| `API_KEY`          | The secret used to sign the data                                    |
| `LOCATION`         | The location where the data was measured                            |
| `MEASURE_INTERVAL` | The time between measurements in seconds                            |

### Installation

```
cd build/esp8266
git submodule update --init
cd tools
python get.py
```

### Build

Run `make build`

  [config_header]: WetterObservatoriumWeb/config.hpp
  [git_wo]: https://github.com/kalehmann/WetterObservatoriumWeb
