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


#include <string.h>

#include "SDS011DataPacket.hpp"

using SDS011lib::SDS011Command;

SDS011lib::SDS011DataPacket::SDS011DataPacket(void) {
  this->checksum = 0;
  this->commandId = NoCommand;
  this->dataLength = 15;
  this->head = SDS011_HEAD;
  this->tail = SDS011_TAIL;
  memset(this->data, 0, this->dataLength);
}

SDS011lib::SDS011DataPacket::SDS011DataPacket(const uint8_t * const data,
                                              size_t n) {
  if (n < 5) {
    // This packet seems to be invalid
    this->head = 0;
    this->tail = 0;

    return;
  }
  this->head = data[0];
  this->commandId = static_cast<SDS011Command>(data[1]);
  this->checksum = data[n - 2];
  this->tail = data[n - 1];

  this->dataLength = n - 4;
  memcpy(this->packetBytes, data, n);
}

void SDS011lib::SDS011DataPacket::buildPacket(void) {
  this->checksum = this->calculateChecksum();
  this->packetBytes[0] = this->head;
  this->packetBytes[1] = this->commandId;
  this->packetBytes[this->dataLength + 2] + this->checksum;
  this->packetBytes[this->dataLength + 3] = this->tail;
}

uint8_t SDS011lib::SDS011DataPacket::getChecksum(void) const {
  return this->checksum;
}

enum SDS011Command SDS011lib::SDS011DataPacket::getCommandId(void) const {
  return this->commandId;
}

void SDS011lib::SDS011DataPacket::setCommandId(SDS011Command commandId) {
  this->commandId = commandId;
}

uint8_t SDS011lib::SDS011DataPacket::getDataByte(unsigned int index) const {
  return this->data[index];
}

void SDS011lib::SDS011DataPacket::setDataByte(unsigned int index,
                                              uint8_t value) {
  this->data[index] = value;
}


uint16_t SDS011lib::SDS011DataPacket::getDeviceId(void) const {
  uint8_t highByte = this->data[this->dataLength - 2];
  uint8_t lowByte = this->data[this->dataLength - 1];

  return lowByte + (highByte * 256);
}

void SDS011lib::SDS011DataPacket::setDeviceId(uint16_t id) {
  uint8_t lowByte = id % 256;
  uint8_t highByte = id / 256;

  this->data[this->dataLength - 2] = highByte;
  this->data[this->dataLength - 1] = lowByte;
}

bool SDS011lib::SDS011DataPacket::isValid(void) const{
  if (this->head != SDS011_HEAD) {
    return false;
  }

  if (this->tail != SDS011_TAIL) {
    return false;
  }

  return this->checksumIsValid();
}

const uint8_t *SDS011lib::SDS011DataPacket::getDataArray(
        size_t * const length) const {
  *length = this->dataLength + 4;

  return this->packetBytes;
}

uint8_t SDS011lib::SDS011DataPacket::calculateChecksum(void) const {
  uint8_t checksum = 0;

  for (unsigned int i = 0; i < this->dataLength; i++) {
    checksum += this->data[i];
  }

  return checksum;
}

bool SDS011lib::SDS011DataPacket::checksumIsValid(void) const {
  return this->checksum == this->calculateChecksum();
}
