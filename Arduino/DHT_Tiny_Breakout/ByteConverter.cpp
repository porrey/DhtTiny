#include "ByteConverter.h"

// *********************
// ***
// *** float
// ***
// *********************
void ByteConverter::floatToBytes(float value, uint8_t* data)
{
  u_float uvalue = u_float();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
  data[2] = uvalue.bytes[2];
  data[3] = uvalue.bytes[3];
}

float ByteConverter::bytesToFloat(uint8_t* data)
{
  u_float uvalue = u_float();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];
  uvalue.bytes[2] = data[2];
  uvalue.bytes[3] = data[3];

  return uvalue.value;
}

// *********************
// ***
// *** double
// ***
// *********************
void ByteConverter::doubleToBytes(double value, uint8_t* data)
{
  u_double uvalue = u_double();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
  data[2] = uvalue.bytes[2];
  data[3] = uvalue.bytes[3];
}

float ByteConverter::bytesToDouble(uint8_t* data)
{
  u_double uvalue = u_double();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];
  uvalue.bytes[2] = data[2];
  uvalue.bytes[3] = data[3];

  return uvalue.value;
}

// *********************
// ***
// *** int64_t
// ***
// *********************
void ByteConverter::int64ToBytes(int64_t value, uint8_t* data)
{
  u_int64 uvalue = u_int64();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
  data[2] = uvalue.bytes[2];
  data[3] = uvalue.bytes[3];
}

int64_t ByteConverter::bytesToInt64(uint8_t* data)
{
  u_int64 uvalue = u_int64();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];
  uvalue.bytes[2] = data[2];
  uvalue.bytes[3] = data[3];

  return uvalue.value;
}

// *********************
// ***
// *** uint64_t
// ***
// *********************
void ByteConverter::uint64ToBytes(uint64_t value, uint8_t* data)
{
  u_uint64 uvalue = u_uint64();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
  data[2] = uvalue.bytes[2];
  data[3] = uvalue.bytes[3];
}

uint64_t ByteConverter::bytesToUint64(uint8_t* data)
{
  u_uint64 uvalue = u_uint64();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];
  uvalue.bytes[2] = data[2];
  uvalue.bytes[3] = data[3];

  return uvalue.value;
}

// *********************
// ***
// *** int32_t
// ***
// *********************
void ByteConverter::int32ToBytes(int32_t value, uint8_t* data)
{
  u_int32 uvalue = u_int32();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
  data[2] = uvalue.bytes[2];
  data[3] = uvalue.bytes[3];
}

int32_t ByteConverter::bytesToInt32(uint8_t* data)
{
  u_int32 uvalue = u_int32();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];
  uvalue.bytes[2] = data[2];
  uvalue.bytes[3] = data[3];

  return uvalue.value;
}

// *********************
// ***
// *** uint32_t
// ***
// *********************
void ByteConverter::uint32ToBytes(uint32_t value, uint8_t* data)
{
  u_uint32 uvalue = u_uint32();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
  data[2] = uvalue.bytes[2];
  data[3] = uvalue.bytes[3];
}

uint32_t ByteConverter::bytesToUint32(uint8_t* data)
{
  u_uint32 uvalue = u_uint32();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];
  uvalue.bytes[2] = data[2];
  uvalue.bytes[3] = data[3];

  return uvalue.value;
}

// *********************
// ***
// *** int16_t
// ***
// *********************
void ByteConverter::int16ToBytes(int16_t value, uint8_t* data)
{
  u_int16 uvalue = u_int16();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
}

int16_t ByteConverter::bytesToInt16(uint8_t* data)
{
  u_int16 uvalue = u_int16();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];

  return uvalue.value;
}

// *********************
// ***
// *** uint16_t
// ***
// *********************
void ByteConverter::uint16ToBytes(uint16_t value, uint8_t* data)
{
  u_uint16 uvalue = u_uint16();
  uvalue.value = value;

  data[0] = uvalue.bytes[0];
  data[1] = uvalue.bytes[1];
}

uint16_t ByteConverter::bytesToUint16(uint8_t* data)
{
  u_uint16 uvalue = u_uint16();

  uvalue.bytes[0] = data[0];
  uvalue.bytes[1] = data[1];

  return uvalue.value;
}
