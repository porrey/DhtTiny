// Copyright © 2016 Daniel Porrey. All Rights Reserved.
//
// This file is part of the DHT Tiny project.
// 
// DHT Tiny is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DHT Tiny is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with DHT Tiny. If not, 
// see http://www.gnu.org/licenses/.
//
#ifndef REGISTERS_H
#define REGISTERS_H

// ***
// *** This file defines the registers and
// *** must include a definition called
// *** REGISTER_TOTAL_SIZE.
// ***
#include "Register_Defs.h"

// ***
// *** Registers.
// ***
volatile uint8_t _registers[REGISTER_TOTAL_SIZE];
volatile uint8_t _registerPosition = 0;

void advanceRegisterPosition()
{
  _registerPosition++;
  if (_registerPosition == REGISTER_TOTAL_SIZE) _registerPosition = 0;  
}

void setRegisterBit(uint8_t registerId, uint8_t statusBit, uint8_t value)
{
  bitWrite(_registers[registerId], statusBit, value);
}

bool getRegisterBit(uint8_t registerId, uint8_t statusBit)
{
  // ***
  // *** Return true if the bit is 1.
  // ***
  return (bitRead(_registers[registerId], statusBit) == 1);
}

float readFloat(uint8_t registerId)
{
  byte data[4];
  data[0] = _registers[registerId + 0];
  data[1] = _registers[registerId + 1];
  data[2] = _registers[registerId + 2];
  data[3] = _registers[registerId + 3];
  return ByteConverter::bytesToFloat(data);
}

void writeFloat(uint8_t registerId, float value)
{
  byte data[4];
  ByteConverter::floatToBytes(value, data);
  _registers[registerId + 0] = data[0];
  _registers[registerId + 1] = data[1];
  _registers[registerId + 2] = data[2];
  _registers[registerId + 3] = data[3];
}

uint32_t readUint32(uint8_t registerId)
{
  byte data[4];
  data[0] = _registers[registerId + 0];
  data[1] = _registers[registerId + 1];
  data[2] = _registers[registerId + 2];
  data[3] = _registers[registerId + 3];
  return ByteConverter::bytesToUint32(data);
}

void writeUint32(uint8_t registerId, uint32_t value)
{
  byte data[4];
  ByteConverter::uint32ToBytes(value, data);
  _registers[registerId + 0] = data[0];
  _registers[registerId + 1] = data[1];
  _registers[registerId + 2] = data[2];
  _registers[registerId + 3] = data[3];
}
#endif
