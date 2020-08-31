/* libSDS011
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

#ifndef WETTEROBSERVATORIUMESP8266_SRC_SDS011_SDS011_HPP_
#define WETTEROBSERVATORIUMESP8266_SRC_SDS011_SDS011_HPP_

#include <stdint.h>
#include <SoftwareSerial.h>

#include "SDS011DataPacket.hpp"

namespace SDS011lib {
        /**
         * Reporting modes for the sensor.
         *
         * With the active mode the sensor reports data by itself in a
         * periodical interval. With the query mode the sensor reports data
         * only when asked.
         */
        enum SDS011_REPORTING_MODE: uint8_t {
                REPORT_ACTIVE_MODE = 0,
                REPORT_QUERY_MODE = 1
        };

        /**
         * SDS011 air quality sensor implementation.
         */
        class SDS011 {
        public:
                /**
                 * @param rx, tx   the pins used by the SoftwareSerial library
                 *                 for receivinvg and sending data to the SDS011
                 *                 air quality sensor.
                 * @param deviceId the device id of the SDS011 air quality
                 *                 sensor. If the device id is unknown use the
                 *                 default value of 0xffff to communincate
                 *                 with all sensors.
                 */
                SDS011(const int rx, const int tx,
                       const uint16_t deviceId = 0xffff);
                /**
                 * Initialize the serial connection.
                 *
                 * This method initializes the serial connection with the
                 * SoftwareSerial library, Before calling this method, the `rx`
                 * pin must be initialized as input with `pinMode(rx, INPUT)`
                 * and the `tx` pin as output with `pinMode(tx, OUTPUT)`.
                 */
                void begin(void);
                /**
                 * Query the data reporting mode from the sensor.
                 *
                 * There are two reporting modes available:
                 * - REPORT_ACTIVE_MODE: the sensor sends data by itself
                 * - REPORT_QUERY_MODEL: the sensor reports data only after a
                 *                       query
                 *
                 * @param[out] queryMode a pointer to the current query mode
                 * @return     whether the query was successful
                 */
                bool queryDataReportingMode(
                        SDS011_REPORTING_MODE * const queryMode);
                /**
                 * Set the data reporting mode.
                 *
                 * There are two reporting modes available:
                 * - REPORT_ACTIVE_MODE: the sensor sends data by itself
                 * - REPORT_QUERY_MODEL: the sensor reports data only after a
                 *                       query
                 * This setting is still effective after power off.
                 *
                 * @param  queryMode the new query mode.
                 * @return whether the data mode was set succesfully.
                 */
                bool setDataReportingMode(
                        const SDS011_REPORTING_MODE queryMode);
                /**
                 * Query data from the sensor.
                 *
                 * @param[out] pm2_5 particles with a diameter less than 2.5
                 *                   micrometers in  0.1 microgram per cubic
                 *                   metre.
                 * @param[out] pm10  particles with a diameter between 2.5 and
                 *                   10 micrometers in 0.1 microgram per cubic
                 *                   metre.
                 * @return           whether the query was successful
                 */
                bool queryData(uint16_t * const pm2_5, uint16_t * const pm10);
                /**
                 * Query the firmware version of the sensor.
                 *
                 * @param[out] year the last two digist of the year the firmware
                 *             was released
                 * @param[out] month the month of the firmware release
                 * @param[out] day the day of the firmware release
                 * @return     whether the query was successful
                 */
                bool queryFirmwareVersion(uint8_t * const year,
                                          uint8_t * const month,
                                          uint8_t * const day);
                /**
                 * Change the id of the sensor.
                 * 
                 * This setting is still effective after power off
                 *
                 * @param deviceId the new id of the sensor
                 * @return whether the id was changed successfully
                 */
                bool setDeviceId(const uint16_t deviceId);
                /**
                 * Set the sensor to sleep.
                 * 
                 * The will disable the fan and the laser. This increases the
                 * lifetime of the sensor as it is rated for 8000 hours of laser
                 * activity.
                 * This setting is not effective after power off.
                 *
                 * @return whether the sensor was successfully send to sleep
                 */
                bool setSleep(void);
                /**
                 * Wake the sensor up.
                 *
                 * Note that the sensor takes up to 30 seconds before it
                 * reports valid data.
                 * This setting is not effictive after power off.
                 *
                 * @return whether the sensor waked up successfully
                 */
                bool setWork(void);
                /**
                 * Query the period the sensor sends data.
                 *
                 * @param[out] n the period of data reporting in minutes.
                 * @return whether the query was successful
                 */
                bool queryWorkingPeriond(uint8_t* const n);
                /**
                 * Set the working period of the sensor.
                 *
                 * @param n the period the sensor reports data in minutes
                 * @return whether the query was successful
                 */
                bool setWorkingPerion(const uint8_t n);

        private:
                /**
                 * Receive a data packet from the sensor.
                 *
                 * @param packet
                 * @param n      the number of bytes that are expected from the
                 *               sensor
                 * @return       whether the data was received successfully
                 */
                SDS011DataPacket receiveData(const size_t n);
                /**
                 * Send a data packet to the sensor
                 *
                 * @param packet
                 * @return whether sending the data was successful
                 */
                bool sendData(const SDS011DataPacket& packet);

                /**
                 * Id of the sensor or 0xffff for any sensor.
                 */
                uint16_t deviceId;
                /**
                 * Instance of the SoftwareSerial library used for communication
                 * with the sensor.
                 */
                SoftwareSerial serial;
        };
}  // namespace SDS011lib

#endif  // WETTEROBSERVATORIUMESP8266_SRC_SDS011_SDS011_HPP_
