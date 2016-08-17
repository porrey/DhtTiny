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

uint8_t calculateCrc8(uint8_t *data, byte byteCount)
{
  uint8_t crc = 0x00;

  while (byteCount--)
  {
    uint8_t extract = *data++;

    for (uint8_t tempI = 8; tempI; tempI--)
    {
      uint8_t sum = (crc ^ extract) & 0x01;
      crc >>= 1;

      if (sum)
      {
        crc ^= 0x8C;
      }

      extract >>= 1;
    }
  }

  return crc;
}
#endif
