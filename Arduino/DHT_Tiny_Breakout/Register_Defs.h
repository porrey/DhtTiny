#ifndef REGISTER_DEFS_H
#define REGISTER_DEFS_H

// ***
// *** Address of each variable
// *** within the registers.
// ***
#define REGISTER_TEMPERATURE        0                                          // *** float
#define REGISTER_HUMIDITY           REGISTER_TEMPERATURE      + SIZE_FLOAT     // *** float
#define REGISTER_INTERVAL           REGISTER_HUMIDITY         + SIZE_FLOAT     // *** uint32
#define REGISTER_READING_ID         REGISTER_INTERVAL         + SIZE_UINT32    // *** uint32
#define REGISTER_UPPER_THRESHOLD    REGISTER_READING_ID       + SIZE_UINT32    // *** float
#define REGISTER_LOWER_THRESHOLD    REGISTER_UPPER_THRESHOLD  + SIZE_FLOAT     // *** float
#define REGISTER_START_DELAY        REGISTER_LOWER_THRESHOLD  + SIZE_FLOAT     // *** uint32
#define REGISTER_CONFIG             REGISTER_START_DELAY      + SIZE_UINT32    // *** uint8
#define REGISTER_STATUS             REGISTER_CONFIG           + SIZE_UINT8     // *** uint8

// ***
// *** Total size of the registers in bytes.
// ***
#define REGISTER_TOTAL_SIZE         REGISTER_STATUS           + SIZE_UINT8

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
// *** 0: Writable, indicates start position
// *** 1: Writable, indicates non-start position
// *** 2: Read-only
// ***
const uint8_t _registerProtection[REGISTER_TOTAL_SIZE] =
{
  2, 2, 2, 2, //REGISTER_TEMPERATURE (read-only)
  2, 2, 2, 2, //REGISTER_HUMIDITY (read-only)
  0, 1, 1, 1, //REGISTER_INTERVAL
  2, 2, 2, 2, //REGISTER_READING_ID (read-only)
  0, 1, 1, 1, //REGISTER_UPPER_THRESHOLD
  0, 1, 1, 1, //REGISTER_LOWER_THRESHOLD
  0, 1, 1, 1, //REGISTER_START_DELAY
  0,          //REGISTER_CONFIG
  2,          //REGISTER_STATUS (read-only)
};

#endif
