#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Registers.h"

// ***
// *** Headers
// ***
void saveUint32(uint8_t address, uint8_t registerId);
void saveFloat(uint8_t address, uint8_t registerId);
uint32_t calculateCrc();

// ***
// *** Defines a signature that can be used to
// *** indicate that the configuration has been saved
// *** to EEPROM.
// ***
#define SIGNATURE     B10101010

// ***
// *** Not all of the configuration bits can be saved. This
// *** mask ou those bits that should not be saved.
// ***
#define CONFIG_MASK   B00000011

void resetConfiguration()
{
  // ***
  // *** Set
  // ***
  for (int i = 0 ; i < (REGISTER_TOTAL_SIZE + 1) ; i++)
  {
    if (EEPROM.read(i) != 0)
    {
      EEPROM.write(i, 0);
    }
  }

  // ***
  // *** Set the status bit.
  // ***
  setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED, 0);
}

void saveConfiguration()
{
  // ***
  // *** Write the signature to the first byte of memory to indicate
  // *** that the configuration was saved.
  // ***
  EEPROM.write(REGISTER_TOTAL_SIZE, SIGNATURE);

  // ***
  // *** Save the registers.
  // ***
  saveUint32(REGISTER_INTERVAL, REGISTER_INTERVAL);
  saveFloat(REGISTER_UPPER_THRESHOLD, REGISTER_UPPER_THRESHOLD);
  saveFloat(REGISTER_LOWER_THRESHOLD, REGISTER_LOWER_THRESHOLD);
  saveUint32(REGISTER_START_DELAY, REGISTER_START_DELAY);
  EEPROM.update(REGISTER_CONFIG, _registers[REGISTER_CONFIG]);

  // ***
  // *** Calculate a CRC and store it after the SIGNATURE byte.
  // ***
  uint32_t crc = calculateCrc();
  saveUint32(REGISTER_TOTAL_SIZE + 1, crc);

  // ***
  // *** Set the status bit.
  // ***
  setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED, 1);
}

bool restoreConfiguration()
{
  bool returnValue = false;

  // ***
  // *** Check the EEPROM for the signature.
  // ***
  uint8_t signature = EEPROM.read(REGISTER_TOTAL_SIZE);

  if (signature == SIGNATURE)
  {
    // ***
    // *** Calculate a CRC.
    // ***
    uint32_t calculatedCrc = calculateCrc();

    // ***
    // *** Read the CRC from EEPROM.
    // ***
    uint32_t storedCrc = 0;
    EEPROM.get(REGISTER_TOTAL_SIZE + 1, storedCrc);

    // ***
    // *** Check the stored value against the calculated value.
    // ***
    if (storedCrc == calculatedCrc)
    {
      // ***
      // *** It is safe to restore.
      // ***
      EEPROM.get(REGISTER_INTERVAL, _registers[REGISTER_INTERVAL]);
      EEPROM.get(REGISTER_UPPER_THRESHOLD, _registers[REGISTER_UPPER_THRESHOLD]);
      EEPROM.get(REGISTER_LOWER_THRESHOLD, _registers[REGISTER_LOWER_THRESHOLD]);
      EEPROM.get(REGISTER_START_DELAY, _registers[REGISTER_START_DELAY]);
      EEPROM.get(REGISTER_CONFIG, _registers[REGISTER_CONFIG]);

      // ***
      // *** Set the status bit.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED, 1);
      returnValue = true;
    }
  }

  return returnValue;
}

void saveUint32(uint8_t address, uint8_t registerId)
{
  // ***
  // *** Get the current register value.
  // ***
  uint32_t value1 = readUint32(REGISTER_INTERVAL);

  // ***
  // *** Define a variable to retrieve the current
  // *** value from EEPROM.
  // ***
  uint32_t value2 = 0;

  // ***
  // *** Read the current value.
  // ***
  EEPROM.get(address, value2);

  // ***
  // *** Check if the current value is different
  // *** from the new value.
  // ***
  if (value1 != value2)
  {
    // ***
    // *** The value is different so update EEPROM.
    // ***
    EEPROM.put(address, value1);
  }
}

void saveFloat(uint8_t address, uint8_t registerId)
{
  // ***
  // *** Get the current register value.
  // ***
  float value1 = readUint32(REGISTER_INTERVAL);

  // ***
  // *** Define a variable to retrieve the current
  // *** value from EEPROM.
  // ***
  float value2 = 0;

  // ***
  // *** Read the current value.
  // ***
  EEPROM.get(address, value2);

  // ***
  // *** Check if the current value is different
  // *** from the new value.
  // ***
  if (value1 != value2)
  {
    // ***
    // *** The value is different so update EEPROM.
    // ***
    EEPROM.put(address, value1);
  }
}

uint32_t calculateCrc()
{
  const unsigned long crc_table[16] =
  {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  // ***
  // *** Don't include the CRC value stored at REGISTER_TOTAL_SIZE + 1.
  // ***
  for (int i = 0 ; i < REGISTER_TOTAL_SIZE ; i++)
  {
    crc = crc_table[(crc ^ EEPROM[i]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[i] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }

  return crc;
}
#endif
