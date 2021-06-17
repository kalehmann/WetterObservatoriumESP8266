BUILD_EXTRA_FLAGS=-DWITH_SERIAL_OUTPUT
THIS_DIR := $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(THIS_DIR)/..
LIBS =  \
  $(ROOT)/vendor/ADS1115_WE \
  $(ROOT)/vendor/BME280 \
  $(ROOT)/vendor/esp8266/libraries \
  $(ROOT)/vendor/esp8266/libraries/ESP8266HTTPClient/src \
  $(ROOT)/vendor/esp8266/libraries/ESP8266WiFi/src \
  $(ROOT)/vendor/esp8266/libraries/Wire \
  $(ROOT)/vendor/NTPClient \
  $(ROOT)/vendor/SimpleDHT

UPLOAD_SPEED = 115200
