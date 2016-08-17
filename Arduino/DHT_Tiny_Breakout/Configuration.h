#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Registers.h"

// ***
// *** Defines a signature that can be used to
// *** indicate that the configuration has been saved
// *** to EEPROM.
// ***
#define SIGNATURE     B10101010

// ***
// *** Not all of the configuration bits can be saved. This
// *** will mask out the bits that should not be saved.
// ***
#define CONFIG_MASK   B00000011

#define START_REGISTER      REGISTER_INTERVAL
#define TOTAL_LENGTH        17

void resetConfiguration()
{
  // ***
  // *** Set
  // ***
  for (int i = 0 ; i < (TOTAL_LENGTH + 1) ; i++)
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
  EEPROM.update(0, SIGNATURE);

  // ***
  // *** Copy a portion of the registers to the ERPROM.
  // ***
  for (int i = 1 ; i < TOTAL_LENGTH - 1 ; i++)
  {
    EEPROM.update(i, _registers[START_REGISTER + (i - 1)]);
  }

  // ***
  // *** Save the config bits.
  // ***
  EEPROM.update(TOTAL_LENGTH - 1, _registers[REGISTER_CONFIG] & CONFIG_MASK);

  // ***
  // *** Set the status bit to indicate that there
  // *** is a valid saved confgiuration.
  // ***
  setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED, 1);
}

bool restoreConfiguration()
{
  bool returnValue = false;

  // ***
  // *** Check the EEPROM for the signature.
  // ***
  uint8_t signature = EEPROM.read(0);

  if (signature == SIGNATURE)
  {
    // ***
    // *** Copy a portion of the registers from the ERPROM.
    // ***
    for (int i = 1 ; i < TOTAL_LENGTH - 1 ; i++)
    {
      _registers[START_REGISTER + (i - 1)] = EEPROM.read(i);
    }

    // ***
    // *** Restore the config bits.
    // ***
    _registers[REGISTER_CONFIG] = EEPROM.read(TOTAL_LENGTH - 1);

    // ***
    // *** Set the status bit.
    // ***
    setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED, 1);
    returnValue = true;
  }

  return returnValue;
}
#endif
