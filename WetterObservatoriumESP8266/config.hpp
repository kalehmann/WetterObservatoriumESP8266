/* WetterObservatoriumESP8266
   Copyright (C) 2021 Karsten Lehmann <mail@kalehmann.de>

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

#ifndef WETTEROBSERVATORIUMESP8266_CONFIG_HPP_
#define WETTEROBSERVATORIUMESP8266_CONFIG_HPP_

static const char* WIFI_SSID = "MY MIFI SSID";

static const char* WIFI_PASS = "*****";

static const char* API_HOST = "alaunwetter.kalehmann.de";

static const uint16_t API_PORT = 80;

static const char* API_KEY = "*****";

static const char* LOCATION = "outdoor";

static const unsigned long MEASURE_INTERVAL = 5 * 60;

#endif  // WETTEROBSERVATORIUMESP8266_CONFIG_HPP_
