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
#ifndef BYTE_CONVERTER_H
#define BYTE_CONVERTER_H

#include <Arduino.h>

#define SIZE_FLOAT    4
#define SIZE_DOUBLE   4
#define SIZE_INT8     1
#define SIZE_UINT8    1
#define SIZE_INT16    2
#define SIZE_UINT16   2
#define SIZE_INT32    4
#define SIZE_UINT32   4
#define SIZE_INT64    8
#define SIZE_UINT64   8

union u_float
{
  uint8_t bytes[SIZE_FLOAT];
  float value;
};

union u_double
{
  uint8_t bytes[SIZE_DOUBLE];
  double value;
};

union u_int64
{
  uint8_t bytes[SIZE_INT64];
  int64_t value;
};

union u_uint64
{
  uint8_t bytes[SIZE_UINT64];
  uint64_t value;
};

union u_int32
{
  uint8_t bytes[SIZE_INT32];
  int32_t value;
};

union u_uint32
{
  uint8_t bytes[SIZE_UINT32];
  uint32_t value;
};

union u_int16
{
  uint8_t bytes[SIZE_INT16];
  int16_t value;
};

union u_uint16
{
  uint8_t bytes[SIZE_UINT16];
  uint16_t value;
};

class ByteConverter
{
  public:
    static void floatToBytes(float value, uint8_t* data);
    static float bytesToFloat(uint8_t* data);

    static void doubleToBytes(double value, uint8_t* data);
    static float bytesToDouble(uint8_t* data);

    static int64_t bytesToInt64(uint8_t* data);
    static void int64ToBytes(int64_t value, uint8_t* data);
    static uint64_t bytesToUint64(uint8_t* data);
    static void uint64ToBytes(uint64_t value, uint8_t* data);

    static int32_t bytesToInt32(uint8_t* data);
    static void int32ToBytes(int32_t value, uint8_t* data);
    static uint32_t bytesToUint32(uint8_t* data);
    static void uint32ToBytes(uint32_t value, uint8_t* data);

    static int16_t bytesToInt16(uint8_t* data);
    static void int16ToBytes(int value, uint8_t* data);
    static uint16_t bytesToUint16(uint8_t* data);
    static void uint16ToBytes(uint16_t value, uint8_t* data);
};
#endif
