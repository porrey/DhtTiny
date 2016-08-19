#ifndef REGISTER_DEFS_H
#define REGISTER_DEFS_H

#include "ByteConverter.h"

// ***
// *** Address of each variable
// *** within the registers.
// ***
#define REGISTER_ID                 0                                          // *** uint8
#define REGISTER_VER_MAJOR          REGISTER_ID               + SIZE_UINT8     // *** uint8
#define REGISTER_VER_MINOR          REGISTER_VER_MAJOR        + SIZE_UINT8     // *** uint8
#define REGISTER_VER_BUILD          REGISTER_VER_MINOR        + SIZE_UINT8     // *** uint8
#define REGISTER_TEMPERATURE        REGISTER_VER_BUILD        + SIZE_UINT8     // *** float
#define REGISTER_HUMIDITY           REGISTER_TEMPERATURE      + SIZE_FLOAT     // *** float
#define REGISTER_STATUS             REGISTER_HUMIDITY         + SIZE_FLOAT     // *** uint8
#define REGISTER_READING_ID         REGISTER_STATUS           + SIZE_UINT8     // *** uint32
#define REGISTER_INTERVAL           REGISTER_READING_ID       + SIZE_UINT32    // *** uint32
#define REGISTER_UPPER_THRESHOLD    REGISTER_INTERVAL         + SIZE_UINT32    // *** float
#define REGISTER_LOWER_THRESHOLD    REGISTER_UPPER_THRESHOLD  + SIZE_FLOAT     // *** float
#define REGISTER_START_DELAY        REGISTER_LOWER_THRESHOLD  + SIZE_FLOAT     // *** uint32
#define REGISTER_CONFIG             REGISTER_START_DELAY      + SIZE_UINT32    // *** uint8
#define REGISTER_DEVICE_ADDRESS     REGISTER_CONFIG           + SIZE_UINT8     // *** uint8

// ***
// *** Total size of the registers in bytes.
// ***
#define REGISTER_TOTAL_SIZE         REGISTER_DEVICE_ADDRESS   + SIZE_UINT8

// ***
// *** This array indicates the number of bytes to return when a read
// *** request is made. If the register adress is aligned to the a
// *** startable address, then the correct number of bytes will be
// *** returned. If not, the byte count returned is zero.
// ***
const uint8_t _registerSize[REGISTER_TOTAL_SIZE] = 
                                      {
                                        SIZE_UINT8,
                                        SIZE_UINT8,
                                        SIZE_UINT8,
                                        SIZE_UINT8,
                                        SIZE_FLOAT, 0, 0, 0,
                                        SIZE_FLOAT, 0, 0, 0,
                                        SIZE_UINT8,
                                        SIZE_UINT32, 0, 0, 0,
                                        SIZE_UINT32, 0, 0, 0,
                                        SIZE_FLOAT, 0, 0, 0,
                                        SIZE_FLOAT, 0, 0, 0,
                                        SIZE_UINT32, 0, 0, 0,
                                        SIZE_UINT8,
                                        SIZE_UINT8,
                                      };
                                                     
// ***
// *** Configuration bits.
// ***
#define CONFIG_BIT_SENSOR_ENABLED           0
#define CONFIG_BIT_THRESHOLD_ENABLED        1
#define CONFIG_BIT_TRIGGER_READING          2
#define CONFIG_BIT_RESERVED_1               3
#define CONFIG_BIT_RESERVED_2               4
#define CONFIG_BIT_RESERVED_3               5
#define CONFIG_BIT_WRITE_CONFIG             6
#define CONFIG_BIT_RESET_CONFIG             7

// ***
// *** Status register bits.
// ***
#define STATUS_SENSOR_IS_ENABLED            0
#define STATUS_UPPER_THRESHOLD_EXCEEDED     1
#define STATUS_LOWER_THRESHOLD_EXCEEDED     2
#define STATUS_RESERVED_1                   3
#define STATUS_RESERVED_2                   4
#define STATUS_CONFIG_SAVED                 5
#define STATUS_READ_ERROR                   6
#define STATUS_WRITE_ERROR                  7

// ***
// *** This array indicates the protection status
// *** for each byte in the register array.
// ***
// *** 0: Writable, indicates a startable position
// *** 1: Writable, indicates a non-startable position
// *** 2: Read-only, indicates a startable position
// *** 3: Read-only, indicates a non-startable position
// ***
const uint8_t _registerProtection[REGISTER_TOTAL_SIZE] =
{
  2,          //REGISTER_ID (read-only)
  2,          //REGISTER_VER_MAJOR (read-only)
  2,          //REGISTER_VER_MINOR (read-only)
  2,          //REGISTER_VER_BUILD (read-only)
  2, 3, 3, 3, //REGISTER_TEMPERATURE (read-only)
  2, 3, 3, 3, //REGISTER_HUMIDITY (read-only)
  2,          //REGISTER_STATUS (read-only)
  2, 3, 3, 3, //REGISTER_READING_ID (read-only)
  0, 1, 1, 1, //REGISTER_INTERVAL
  0, 1, 1, 1, //REGISTER_UPPER_THRESHOLD
  0, 1, 1, 1, //REGISTER_LOWER_THRESHOLD
  0, 1, 1, 1, //REGISTER_START_DELAY
  0,          //REGISTER_CONFIG
  0           //REGISTER_DEVICE_ADDRESS
};

#endif
