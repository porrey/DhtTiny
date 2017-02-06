#include <Arduino.h>

#ifndef BYTE_CONVERTER_H
#define BYTE_CONVERTER_H

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
  byte bytes[SIZE_FLOAT];
  float value;
};

union u_double
{
  byte bytes[SIZE_DOUBLE];
  double value;
};

union u_int64
{
  byte bytes[SIZE_INT64];
  int64_t value;
};

union u_uint64
{
  byte bytes[SIZE_UINT64];
  uint64_t value;
};

union u_int32
{
  byte bytes[SIZE_INT32];
  int32_t value;
};

union u_uint32
{
  byte bytes[SIZE_UINT32];
  uint32_t value;
};

union u_int16
{
  byte bytes[SIZE_INT16];
  int16_t value;
};

union u_uint16
{
  byte bytes[SIZE_UINT16];
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
