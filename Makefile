CC=g++
CC_FLAGS=-std=c++11 -Wall
CC_TEST_FLAGS=-I vendor/catch2/single_include

## makeEspArduino specific stuff
BUILD_DIR=build/
ESP_ROOT=vendor/esp8266
MAKEFILE=vendor/makeEspArduino/makeEspArduino.mk
SKETCH=WetterObservatoriumESP8266/WetterObservatoriumESP8266.ino

export BUILD_DIR
export ESP_ROOT
export SKETCH

SOURCES = \
	WetterObservatoriumESP8266/WetterObservatoriumESP8266.ino \
	WetterObservatoriumESP8266/src/sds011/SDS011.cpp \
	WetterObservatoriumESP8266/src/sds011/SDS011.h \
	WetterObservatoriumESP8266/src/sds011/SDS011DataPacket.cpp \
	WetterObservatoriumESP8266/src/sds011/SDS011DataPacket.h

TEST_BINARY = tests/check
TEST_SOURCES = tests/main.cpp

$(TEST_BINARY): $(TEST_SOURCES)
	$(CC) $(CC_FLAGS) $(CC_TEST_FLAGS) -o $(TEST_BINARY) $(TEST_SOURCES)

build: $(SOURCES)
	$(MAKE) -f $(MAKEFILE)

check: $(TEST_BINARY)
	$(TEST_BINARY) --success

clean:
	find build/ -mindepth 1 -not -path build/.gitignore | xargs -0 rm -rf
	rm -rf $(TEST_BINARY)

help:
	$(MAKE) -f $(MAKEFILE) help

.PHONY: build check clean help
