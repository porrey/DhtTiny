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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Registers.h"

// ***
// *** Supported DHT Models
// ***
#define DHT_MODEL_11          11
#define DHT_MODEL_21          21
#define DHT_MODEL_22          22
#define DHT_MODEL_33          33
#define DHT_MODEL_44          44
#define DHT_MODEL_DEFAULT     DHT_MODEL_22

// ***
// *** Default slave address.
// ***
#define I2C_SLAVE_ADDRESS 0x26

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

// ***
// *** Locations of the configuration.
// ***
#define START_REGISTER      REGISTER_INTERVAL
#define TOTAL_LENGTH        17

// ***
// *** Locations of the device address.
// ***
#define ADDRESS_SIGNATURE   TOTAL_LENGTH + 2
#define DEVICE_ADDRESS      TOTAL_LENGTH + 3
#define MODEL_SIGNATURE     TOTAL_LENGTH + 4
#define DHT_MODEL           TOTAL_LENGTH + 5

uint8_t getDeviceAddress()
{
  uint8_t returnValue = I2C_SLAVE_ADDRESS;

  // ***
  // *** If the memory at address ADDRESS_SIGNATURE has the
  // *** SIGNATURE byte, then we know the next position has a valid
  // *** address.
  // ***
  if (EEPROM.read(ADDRESS_SIGNATURE) == SIGNATURE)
  {
    returnValue = EEPROM.read(DEVICE_ADDRESS);
  }
  else
  {
    EEPROM.update(ADDRESS_SIGNATURE, SIGNATURE);
    EEPROM.update(DEVICE_ADDRESS, I2C_SLAVE_ADDRESS);
  }

  return returnValue;
}

void setDeviceAddress(byte address)
{
  EEPROM.update(ADDRESS_SIGNATURE, SIGNATURE);
  EEPROM.update(DEVICE_ADDRESS, address);
}

void resetDeviceAddress()
{
  EEPROM.update(ADDRESS_SIGNATURE, 0);
  EEPROM.update(DEVICE_ADDRESS, 0);
}

uint8_t getDhtModel()
{
  uint8_t returnValue = DHT_MODEL_DEFAULT;

  // ***
  // *** If the memory at address ADDRESS_SIGNATURE has the
  // *** SIGNATURE byte, then we know the next position has a valid
  // *** address.
  // ***
  if (EEPROM.read(MODEL_SIGNATURE) == SIGNATURE)
  {
    returnValue = EEPROM.read(DHT_MODEL);
  }
  else
  {
    EEPROM.update(MODEL_SIGNATURE, SIGNATURE);
    EEPROM.update(DHT_MODEL, DHT_MODEL_DEFAULT);
  }

  return returnValue;
}

void setDhtModel(byte model)
{
  EEPROM.update(MODEL_SIGNATURE, SIGNATURE);
  EEPROM.update(DHT_MODEL, model);
}

void resetDhtModel()
{
  EEPROM.update(MODEL_SIGNATURE, 0);
  EEPROM.update(DHT_MODEL, 0);
}

void resetConfiguration()
{
  // ***
  // *** Set
  // ***
  for (uint16_t i = 0 ; i < (TOTAL_LENGTH + 1) ; i++)
  {
    EEPROM.update(i, 0);
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
  for (uint16_t i = 1 ; i < TOTAL_LENGTH - 1 ; i++)
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
    for (uint16_t i = 1 ; i < TOTAL_LENGTH - 1 ; i++)
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
