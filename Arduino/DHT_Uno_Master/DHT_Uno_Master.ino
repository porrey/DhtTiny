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
//
// This sketch will "exercise" the DHT Tiny board
// by stepping through various settings and configurations
// while displaying he results in a constant loop.
//
#include <Arduino.h>
#include <Wire.h>
#include "ByteConverter.h"

// ***
// *** DHT Breakout reset pin.
// ***
#define DHT_RESET_PIN 3

// ***
// *** DHT Breakout reset pin.
// ***
#define DHT_INTERRUPT_PIN 4

// ***
// *** DHT Breakout address.
// ***
uint8_t _deviceAddress = 0;

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
#define REGISTER_DHT_MODEL          REGISTER_DEVICE_ADDRESS   + SIZE_UINT8     // *** uint8

// ***
// *** Total size of the registers in bytes.
// ***
#define REGISTER_TOTAL_SIZE         REGISTER_DHT_MODEL        + SIZE_UINT8

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
#define STATUS_DHT_READING_ERROR            3
#define STATUS_RESERVED_2                   4
#define STATUS_CONFIG_SAVED                 5
#define STATUS_READ_ERROR                   6
#define STATUS_WRITE_ERROR                  7

// ***
// *** The amount of delay to use after a write
// *** operation to allow the device time to respond.
// ***
#define DEVICE_DELAY 50

// ***
// *** The amount of time, in milliseconds, to
// *** delay after each loop iteration.
// ***
uint32_t _myInterval = 2000;

void setup()
{
  // ***
  // *** Initialize the Serial.
  // ***
  Serial.begin(115200);

  // ***
  // *** Initialize the reset pin.
  // ***
  pinMode(DHT_RESET_PIN, OUTPUT);
  digitalWrite(DHT_RESET_PIN, HIGH);

  // ***
  // *** Initialize the interrupt pin.
  // ***
  pinMode(DHT_INTERRUPT_PIN, INPUT_PULLUP);

  // ***
  // *** Initialize the I2C.
  // ***
  Wire.begin();
  Wire.setClock(400000);

  // ***
  // *** Give the DHT Tiny a chance to get initialized.
  // ***
  Serial.println(F("Giving the DHT Tiny an opportunity to initialize...")); Serial.println();
  delay(2000);

  // ***
  // *** Get the device address by searching for
  // *** the device on the i2c bus.
  // ***
  Serial.println(F("Searching for DHT Tiny..."));
  _deviceAddress = findFirstDhtTiny();

  // ***
  // *** Check if we have a valid address.
  // ***
  if (_deviceAddress != 0)
  {
    // ***
    // *** A device was found.
    // ***
    Serial.print(F("A DHT Tiny was found at address 0x")); Serial.println(_deviceAddress, HEX);
    Serial.println();

    // ***
    // *** Check the current DHT model and give the user
    // *** an opportunity to change it.
    // ***
    dhtModelCheck();

    // ***
    // *** Display device data.
    // ***
    displayDeviceData();
  }
  else
  {
    // ***
    // *** A device was not found.
    // ***
    Serial.print(F("A DHT Tiny was NOT found. Please connect a device and try again."));
  }
}

// ***
// *** Define the testing states. Each value is the
// *** counter value that will trigger the state.
// ***
#define STATE_INITIAL                0
#define STATE_DISABLE_SENSOR         3
#define STATE_ENSABLE_SENSOR         6
#define STATE_ENABLE_THRESHOLDS      9
#define STATE_DISABLE_THRESHOLDS    12
#define STATE_INTERVAL_1            15
#define STATE_INTERVAL_0            18
#define STATE_INTERVAL_MANUAL       21
#define STATE_RESTORE_INTERVAL      24
#define STATE_BAD_WRITE             27
#define STATE_SAVE_CONFIG           30
#define STATE_RESET                 33

// ***
// *** Keeps track of where we are at in the
// *** test cycle.
// ***
uint32_t _myCounter = 0;

void loop()
{
  if (_deviceAddress != 0)
  {
    switch (_myCounter)
    {
      case STATE_INITIAL:
        {
          // ***
          // *** Reset the saved configuration.
          // ***
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_RESET_CONFIG, 1);
          sendUint8(REGISTER_CONFIG, configValue);

          // ***
          // *** Reboot the device.
          // ***
          rebootDevice();

          Serial.println(F("***************************************************************"));
          Serial.println(F("Setting interval to 2 seconds, checking device every 2 seconds."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          sendUint32(REGISTER_INTERVAL, 2000);
          _myInterval = 2000;
          break;
        }
      case STATE_DISABLE_SENSOR:
        {
          // ***
          // *** Set the enable bit in the configuration to 0.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Disabling the sensor."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_SENSOR_ENABLED, 0);
          sendUint8(REGISTER_CONFIG, configValue);
          break;
        }
      case STATE_ENSABLE_SENSOR:
        {
          // ***
          // *** Set the enable bit in the configuration to 1.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Enabling the sensor."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_SENSOR_ENABLED, 1);
          sendUint8(REGISTER_CONFIG, configValue);
          break;
        }
      case STATE_ENABLE_THRESHOLDS:
        {
          // ***
          // *** Turn the thresholds on.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Enabling thresholds."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));

          // ***
          // *** Set the thresholds
          // ***
          sendFloat(REGISTER_LOWER_THRESHOLD, 19.0);
          sendFloat(REGISTER_UPPER_THRESHOLD, 21.0);

          // ***
          // *** Set the configuration bit to enable the thresholds.
          // ***
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_THRESHOLD_ENABLED, 1);
          sendUint8(REGISTER_CONFIG, configValue);
          break;
        }
      case STATE_DISABLE_THRESHOLDS:
        {
          // ***
          // *** Turn the thresholds off.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Disabling thresholds."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));

          // ***
          // *** Reset the thresholds
          // ***
          sendFloat(REGISTER_LOWER_THRESHOLD, 0.0);
          sendFloat(REGISTER_UPPER_THRESHOLD, 0.0);

          // ***
          // *** Set the configuration bit to disable the thresholds.
          // ***
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_THRESHOLD_ENABLED, 0);
          sendUint8(REGISTER_CONFIG, configValue);
          break;
        }
      case STATE_INTERVAL_1:
        {
          // ***
          // *** Change the interval to 1 second.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Setting interval to 1 second, checking device every 1 second."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          sendUint32(REGISTER_INTERVAL, 1000);
          _myInterval = 1000;
          break;
        }
      case STATE_INTERVAL_0:
        {
          // ***
          // *** Change the interval to manual.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Setting interval to 0, checking device every 2 seconds."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          sendUint32(REGISTER_INTERVAL, 0);
          _myInterval = 2000;
          break;
        }
      case STATE_INTERVAL_MANUAL:
        {
          // ***
          // *** Trigger a reading manually.
          // ***
          Serial.println(F("***************************************************************"));
          Serial.println(F("Triggering a reading manually."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_TRIGGER_READING, 1);
          sendUint8(REGISTER_CONFIG, configValue);

          // ***
          // *** Give the sensor time to take a reading.
          // ***
          delay(350);
          break;
        }
      case STATE_RESTORE_INTERVAL:
        {
          Serial.println(F("***************************************************************"));
          Serial.println(F("Setting interval to 2 seconds, checking device every 2 seconds."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));
          sendUint32(REGISTER_INTERVAL, 2000);
          _myInterval = 2000;
          break;
        }
      case STATE_BAD_WRITE:
        {
          Serial.println(F("***************************************************************"));
          Serial.println(F("Writing to wrong register position."));
          Serial.println(F("***************************************************************"));
          Serial.println(F(""));

          // ***
          // *** The device will reject this update.
          // ***
          sendUint32(REGISTER_INTERVAL + 1, 1000);

          break;
        }
      case STATE_SAVE_CONFIG:
        {
          Serial.println(F("***************************************************************"));
          Serial.println(F("Saving the configuration."));
          Serial.println(F("***************************************************************"));
          Serial.println();

          // ***
          // *** Set the configuration bits to enable
          // *** the sensor and the thresholds.
          // ***
          uint8_t configValue = requestUint8(REGISTER_CONFIG);
          bitWrite(configValue, CONFIG_BIT_SENSOR_ENABLED, 1);
          bitWrite(configValue, CONFIG_BIT_THRESHOLD_ENABLED, 1);
          sendUint8(REGISTER_CONFIG, configValue);

          // ***
          // *** Set the interval
          // ***
          sendUint32(REGISTER_INTERVAL, 1500);

          // ***
          // *** Set the thresholds
          // ***
          sendFloat(REGISTER_LOWER_THRESHOLD, 19.0);
          sendFloat(REGISTER_UPPER_THRESHOLD, 21.0);
          bitWrite(configValue, CONFIG_BIT_THRESHOLD_ENABLED, 0);

          // ***
          // *** Set the configuration bit to indicate we
          // *** want the current configuration saved to EEPROM.
          // ***
          bitWrite(configValue, CONFIG_BIT_WRITE_CONFIG, 1);
          sendUint8(REGISTER_CONFIG, configValue);
          delay(1000);

          // ***
          // *** Display the current settings.
          // ***
          displayData();

          // ***
          // *** Reboot the device.
          // ***
          rebootDevice();
          break;
        }
      case STATE_RESET:
        {
          _myCounter = 0;
          break;
        }
    }

    // ***
    // *** Display the sensor data.
    // ***
    displayData();

    // ***
    // *** Wait
    // ***
    Serial.print(F("\tWaiting ")); Serial.print(_myInterval / 1000); Serial.println(F(" second(s) until next request."));
    Serial.println();
    delay(_myInterval);

    // ***
    // *** Increment the loop counter.
    // ***
    _myCounter++;
  }
}

// ***
// *** Find the first DHT Tiny on the i2c bus and
// *** returns it's address.
// ***
uint8_t findFirstDhtTiny()
{
  uint8_t returnValue = 0;

  for (uint8_t address = 8; address <= 77; address++)
  {
    Wire.beginTransmission(address);
    Wire.write(REGISTER_ID);
    uint8_t result = Wire.endTransmission(true);

    if (result == 0)
    {
      // ***
      // *** A device was found, but make sure it is
      // *** a DHT Tiny by reading the ID register.
      // ***
      Wire.requestFrom((uint8_t)address, (uint8_t)1);
      uint8_t id = Wire.read();

      // ***
      // *** The ID value should be 0x2D for a DHT Tiny.
      // ***
      if (id == 0x2d)
      {
        // ***
        // *** A DHT Tiny was found.
        // ***
        returnValue = address;
        break;
      }
    }
  }

  return returnValue;
}

// ***
// *** Checks the current DHT model set on the DHT Tiny
// *** and gives an opportunity to change it. This only allows
// *** DHT11 or DHT22 even the DHT Tiny can support other models.
// ***
void dhtModelCheck()
{
  // ***
  // *** Get and display the current model from the
  // *** register on the DHT Tiny.
  // ***
  uint8_t model = requestUint8(REGISTER_DHT_MODEL);
  Serial.print(F("The Current DHT model is DHT")); Serial.println(model);

  // ***
  // *** Display a menu for the user.
  // ***
  if (model == 11)
  {
    Serial.println(F("Choose: [1] Use this model [2] Change to a DHT22"));
  }
  else
  {
    Serial.println(F("Choose: [1] Use this model [2] Change to a DHT11"));
  }

  // ***
  // *** Wait for one o the options from
  // *** the serial port.
  // ***
  String option = Serial.readString();
  while (!Serial.available() && option != F("1") && option != F("2"))
  {
    option = Serial.readString();
  }

  // ***
  // *** Check which option was chosen.
  // ***
  if (option == F("1"))
  {
    Serial.println(F("You chose to keep the existing model."));
  }
  else if (option == F("2"))
  {
    // ***
    // *** The user chose to change the model. Check the
    // *** current model and change he device to the other.
    // ***
    if (model == 11)
    {
      Serial.println(F("Changing model to DHT22."));
      sendUint8(REGISTER_DHT_MODEL, 22);
    }
    else
    {
      Serial.println(F("Changing model to DHT11."));
      sendUint8(REGISTER_DHT_MODEL, 11);
    }
  }

  Serial.println();
}

// ***
// *** Reboot the DHT Tiny
// ***
void rebootDevice()
{
  Serial.println(F(""));
  Serial.println(F("************************************************************"));
  Serial.println(F("Rebooting device."));
  Serial.println(F("************************************************************"));
  Serial.println(F(""));
  digitalWrite(DHT_RESET_PIN, LOW);
  delay(350);
  digitalWrite(DHT_RESET_PIN, HIGH);
  delay(2000);
}

// ***
// *** Gets and display information about
// *** the connected DHT Tiny.
// ***
void displayDeviceData()
{
  // ***
  // *** Read the device ID.
  // ***
  uint8_t deviceId = requestUint8(REGISTER_ID);

  // ***
  // *** Read the major version.
  // ***
  uint8_t major = requestUint8(REGISTER_VER_MAJOR);

  // ***
  // *** Read the minor version.
  // ***
  uint8_t minor = requestUint8(REGISTER_VER_MINOR);

  // ***
  // *** Read the build.
  // ***
  uint8_t build = requestUint8(REGISTER_VER_BUILD);

  // ***
  // *** Read the device address.
  // ***
  uint8_t address = requestUint8(REGISTER_DEVICE_ADDRESS);

  // ***
  // *** Read the DHT model.
  // ***
  uint8_t dhtModel = requestUint8(REGISTER_DHT_MODEL);

  // ***
  // *** Display the results.
  // ***
  Serial.println(F("Device Information:"));
  Serial.print(F("\tID = 0x")); Serial.println(deviceId, HEX);
  Serial.print(F("\tFirmware Version = ")); Serial.print(major); Serial.print(F(".")); Serial.print(minor); Serial.print(F(" (Build ")); Serial.print(build); Serial.println(F(")"));
  Serial.print(F("\tDevice Address = 0x")); Serial.println(address, HEX);
  Serial.print(F("\tDHT Model = DHT")); Serial.println(dhtModel);
}

// ***
// *** Display the current values and registers on
// *** the connected DHT Tiny.
// ***
void displayData()
{
  // ***
  // *** Read the temperature.
  // ***
  float temperatureC = requestFloat(REGISTER_TEMPERATURE);
  float temperatureF = temperatureC * 1.8 + 32.0;

  // ***
  // *** Read the humidity.
  // ***
  float humidity = requestFloat(REGISTER_HUMIDITY);

  // ***
  // *** Read the interval.
  // ***
  uint32_t interval = requestUint32(REGISTER_INTERVAL);

  // ***
  // *** Read the start delay.
  // ***
  uint32_t startDelay = requestUint32(REGISTER_START_DELAY);

  // ***
  // *** Read the current reading ID.
  // ***
  uint32_t id = requestUint32(REGISTER_READING_ID);

  // ***
  // *** Read the upper temperature threshold.
  // ***
  float upperThreshold = requestFloat(REGISTER_UPPER_THRESHOLD);

  // ***
  // *** Read the lower temperature threshold.
  // ***
  float lowerThreshold = requestFloat(REGISTER_LOWER_THRESHOLD);

  // ***
  // *** Read the configuration bits.
  // ***
  uint8_t configValue = requestUint8(REGISTER_CONFIG);

  // ***
  // *** Read the status bits.
  // ***
  uint8_t statusValue = requestUint8(REGISTER_STATUS);

  // ***
  // *** Display the results.
  // ***
  Serial.print(F("Reading ID = ")); Serial.println(id);
  Serial.print(F("\tInterval = ")); Serial.print(interval); Serial.println(F(" mS"));
  Serial.print(F("\tStart Delay = ")); Serial.print(startDelay); Serial.println(F(" mS"));
  Serial.print(F("\tTemperature = ")); Serial.print(temperatureC); Serial.print(F("C, ")); Serial.print(temperatureF); Serial.println(F("F"));
  Serial.print(F("\tHumidity = ")); Serial.print(humidity); Serial.println(F("%"));
  Serial.print(F("\tUpper Threshold = ")); Serial.print(upperThreshold); Serial.println(F("C"));
  Serial.print(F("\tLower Threshold = ")); Serial.print(lowerThreshold); Serial.println(F("C"));

  // ***
  // *** Display the configuration bits.
  // ***
  Serial.print(F("\tConfigurations bits: ")); displayBits(configValue); Serial.println();

  // ***
  // *** Display the status bits.
  // ***
  Serial.print(F("\tStatus bits:         ")); displayBits(statusValue); Serial.println();

  // ***
  // *** Display specific bit values.
  // ***
  if (bitRead(configValue, CONFIG_BIT_THRESHOLD_ENABLED) == 1) Serial.println(F("\tThresholds are enabled.")); else Serial.println(F("\tThresholds are disabled."));
  if (bitRead(statusValue, STATUS_UPPER_THRESHOLD_EXCEEDED) == 1) Serial.println(F("\tUpper threshold exceeded."));
  if (bitRead(statusValue, STATUS_LOWER_THRESHOLD_EXCEEDED) == 1) Serial.println(F("\tLower threshold exceeded."));
  if (bitRead(statusValue, STATUS_DHT_READING_ERROR) == 1) Serial.println(F("\tDHT Read Error = YES")); else Serial.println(F("\tDHT Read Error = NO"));
  if (bitRead(configValue, CONFIG_BIT_TRIGGER_READING) == 1) Serial.println(F("\tTrigger Manual Read bit is 1.")); else Serial.println(F("\tTrigger Manual Read bit is 0."));

  // ***
  // *** Read and display the value of the DHT_INTERRUPT_PIN pin. This pin
  // *** should be HIGH whenever the temperature threshold is exceeded.
  // ***
  uint8_t pinValue = digitalRead(DHT_INTERRUPT_PIN);
  Serial.print(F("\tArduino Pin ")); Serial.print(DHT_INTERRUPT_PIN); Serial.print(F(" value is "));
  if (pinValue == LOW) Serial.println(F("LOW")); else Serial.println(F("HIGH"));
}

// ***
// *** Displays the bits of a given byte value.
// ***
void displayBits(uint8_t byteValue)
{
  // ***
  // *** Display the bits with a '-'
  // *** character between each bit.
  // ***
  for (int i = 7; i >= 0; i--)
  {
    Serial.print(bitRead(byteValue, i)); if (i > 0) Serial.print(F("-"));
  }
}

// ***
// *** Request one or more bytes from a given register
// *** on the DHT Tiny.
// ***
void requestBytes(uint8_t registerId, uint8_t byteCount, uint8_t* data)
{
  // ***
  // *** Send the byte array to the i2c device.
  // ***
  Wire.beginTransmission(_deviceAddress);
  Wire.write(registerId);
  byte response = Wire.endTransmission(true);

  if (response == 0)
  {
    Wire.requestFrom(_deviceAddress, byteCount);
    for (int i = 0; i < byteCount; i++)
    {
      data[i] = Wire.read();
    }
  }
}

// ***
// *** Send one or more bytes to a given register
// *** on the DHT Tiny.
// ***
bool sendBytes(uint8_t registerId, uint8_t byteCount, uint8_t* data)
{
  bool returnValue = false;

  // ***
  // *** Send the byte array to the i2c device.
  // ***
  Wire.beginTransmission(_deviceAddress);
  Wire.write(registerId);

  for (int i = 0; i < byteCount; i++)
  {
    Wire.write(data[i]);
  }

  returnValue = (Wire.endTransmission() == 0);

  // ***
  // *** Give the device time to respond.
  // ***
  delay(DEVICE_DELAY);

  return returnValue;
}

// ***
// *** Request a uint8_t from a given register
// *** on the DHT Tiny.
// ***
uint8_t requestUint8(uint8_t registerId)
{
  float returnValue = 0;

  byte data[1];
  requestBytes(registerId, 1, data);
  returnValue = data[0];

  return returnValue;
}

// ***
// *** Send a uint8_t to a given register
// *** on the DHT Tiny.
// ***
bool sendUint8(uint8_t registerId, uint8_t value)
{
  bool returnValue = false;

  byte data[1] = { value };
  returnValue = sendBytes(registerId, 1, data);

  return returnValue;
}

// ***
// *** Request a float from a given register
// *** on the DHT Tiny.
// ***
float requestFloat(uint8_t registerId)
{
  float returnValue = 0;

  byte data[4];
  requestBytes(registerId, 4, data);
  returnValue = ByteConverter::bytesToFloat(data);

  return returnValue;
}

// ***
// *** Send a float to a given register
// *** on the DHT Tiny.
// ***
bool sendFloat(uint8_t registerId, float value)
{
  bool returnValue = false;

  byte data[4];
  ByteConverter::floatToBytes(value, data);
  returnValue = sendBytes(registerId, 4, data);

  return returnValue;
}

// ***
// *** Request a uint32_t from a given register
// *** on the DHT Tiny.
// ***
uint32_t requestUint32(uint8_t registerId)
{
  uint32_t returnValue = 0;

  byte data[4];
  requestBytes(registerId, 4, data);
  returnValue = ByteConverter::bytesToUint32(data);

  return returnValue;
}

// ***
// *** Send a uint32_t to a given register
// *** on the DHT Tiny.
// ***
bool sendUint32(uint8_t registerId, uint32_t value)
{
  bool returnValue = false;

  byte data[4];
  ByteConverter::uint32ToBytes(value, data);
  returnValue = sendBytes(registerId, 4, data);

  return returnValue;
}
