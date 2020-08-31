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

#ifndef WETTEROBSERVATORIUMESP8266_SRC_SDS011_SDS011DATAPACKET_HPP_
#define WETTEROBSERVATORIUMESP8266_SRC_SDS011_SDS011DATAPACKET_HPP_

#include <stdint.h>

#define SDS011_HEAD 0xaa
#define SDS011_TAIL 0xab

namespace SDS011lib {

        /**
         * Commands accepted by the SDS011 air quality sensor.
         */
        enum SDS011Command : uint8_t {
                /**
                 * This is not an actual command.
                 * It marks that no command was set yet.
                 */
                NoCommand = 0,
                /** Request or set the current data reporting mode. */
                DataReportingRequest = 0xb4,
                /** Response with the data reporting mode. */
                DataReportingResponse = 0xc5,
                /** Request air quality data from the sensor. */
                DataRequest = 0xb4,
                /** Response with air quality data from the sensor. */
                DataResponse = 0xc0,
                /** Set the device id of the sensor. */
                DeviceIdRequest = 0xb4,
                /** Response with the new device id of the sensor */
                DeviceIdResponse = 0xc5,
                /** Request or set the current working mode (sleep/work). */
                SleepRequest = 0xb4,
                /** Response with the current working mode */
                SleepResponse = 0xc5,
                /** Request or query the current working period */
                WorkingPeriodRequest = 0xb4,
                /** Response with the current working period */
                WorkingPeriodResponse = 0xc5,
                /** Request the firmware version. */
                FirmwareRequest = 0xb4,
                /** Response with the firmware version. */
                FirmwareResponse = 0xc5
        };

        /**
         * A data packet sent or received from a SDS011 sensor.
         */
        class SDS011DataPacket {
        public:
                /**
                 * Creates a new SDS011DataPacket without any data.
                 */
                SDS011DataPacket(void);
                /**
                 * Creates a SDS011DataPacket from existing data.
                 *
                 * @param data the data for the new data packet
                 * @param length the length of the data
                 */
                SDS011DataPacket(const uint8_t * const data,
                                 const size_t length);
                /**
                 * Make the packet ready to be sent to the sensor.
                 *
                 * The method isValid() will return true after a call to this
                 * method.
                 */
                void buildPacket(void);
                /**
                 * Calculate the checksum of the packet.
                 *
                 * @return the calculated checksum
                 */
                uint8_t getChecksum(void) const;
                /**
                 * Get the id of the command.
                 *
                 * @return the id of the command
                 */
                enum SDS011Command getCommandId(void) const;
                /**
                 * Set the id of the command.
                 *
                 * @param commandId the new id of the comand
                 */
                void setCommandId(const enum SDS011Command commandId);
                /**
                 * Get the data byte at a given index.
                 *
                 * @param  index is the index of the data byte to get.
                 * @return the data byte a the index
                 */
                uint8_t getDataByte(const unsigned int index) const;
                /**
                 * Set the data byte at a given index.
                 *
                 * @param index is the index of the data byte to set.
                 * @param value is the value the data byte will be set to.
                 */
                void setDataByte(const unsigned int index, const uint8_t value);
                /**
                 * Get the device id.
                 *
                 * @return the device id
                 */
                uint16_t getDeviceId(void) const;
                /**
                 * Set the device id.
                 *
                 * @param id the new device id.
                 */
                void setDeviceId(const uint16_t id);
                /**
                 * Check whether the packet has a valid head, tail and checksum.
                 *
                 * @return whether the packet is considered valid
                 */
                bool isValid(void) const;
                /**
                 * Get the data of the packet as byte array.
                 *
                 * @param[out] length a pointer to the length of the data array
                 * @return     the data array
                 */
                const uint8_t *getDataArray(size_t * const length) const;

        private:
                /**
                 * Calculate the checksum for the packet.
                 *
                 * The checksum is the 8 bit sum of all data bytes including the
                 * device id.
                 *
                 * @return the checksum for the packet
                 */
                uint8_t calculateChecksum(void) const;
                /**
                 * Verfify whether the checksum for the packet is valid.
                 *
                 * @return whether the checksum for the packet is valid.
                 */
                bool checksumIsValid(void) const;
                /**
                 * The checksum of the package.
                 */
                uint8_t checksum;
                /**
                 * The id of the command.
                 */
                enum SDS011Command commandId;
                /**
                 * All bytes of the package.
                 */
                uint8_t packetBytes[19];
                /**
                 * The data part of the package.
                 */
                uint8_t *data = &packetBytes[2];
                /**
                 * The length of the data.
                 *
                 * For requests this value is always 19. The value for responses
                 * varies.
                 */
                size_t dataLength;
                /**
                 * The first byte of the data package. 
                 *
                 * For valid packages, this value will always be 0xAA.
                 */
                uint8_t head;
                /**
                 * The last byte of the data package.
                 *
                 * For valid packages, this value will always be 0xAB.
                 */
                uint8_t tail;
        };
}  // namespace SDS011lib

#endif  // WETTEROBSERVATORIUMESP8266_SRC_SDS011_SDS011DATAPACKET_HPP_
