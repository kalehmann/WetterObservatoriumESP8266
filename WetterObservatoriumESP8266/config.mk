THIS_DIR := $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(THIS_DIR)/..
LIBS =  \
  $(ROOT)/vendor/esp8266/libraries

UPLOAD_SPEED = 115200
