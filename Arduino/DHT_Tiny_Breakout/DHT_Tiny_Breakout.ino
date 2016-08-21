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

// ********************************************************************************
// ********************************************************************************
// ***                                                                          ***
// *** TinyWireS                                                                ***
// ***                                                                          ***
// *** Install the TinyWireS library from https://github.com/rambo/TinyWire     ***
// *** into your libraries folder when using the ATtiny.                        ***
// ***                                                                          ***
// ********************************************************************************
// ********************************************************************************

#include "Version.h"
#include "dht.h"
#include "ByteConverter.h"
#include "Registers.h"
#include "Register_Defs.h"
#include "Configuration.h"
#include "MyWire.h"
#include "Pins.h"
#include "Debug.h"

// ***
// *** Default interval used to read the DHT
// *** sensor (in milliseconds).
// ***
#define DEFAULT_UPDATE_INTERVAL 2000

// ***
// *** DHT
// ***
dht _dht;

// ***
// *** Stores the last time a sensor reading was taken.
// ***
uint32_t _lastReading = millis();

// ***
// *** Indicates if the thresholds are currently enabled. This flag is
// *** used to track the last value of the threshold bit to prevent the
// *** need write to the interrupt pin on every loop.
// ***
volatile bool _thresholdsEnabled = false;

// ***
// *** The number of bytes to return in the next request.
// ***
volatile uint8_t _requestCount = 0;

void setup()
{
  initDebug();
  displayRegisters();

  // ***
  // *** Restore the configuration from EEPROM. If the configuration
  // *** could not be restord, set the default values.
  // ***
  if (!restoreConfiguration())
  {
    // ***
    // *** Set the default interval in the registers.
    // ***
    writeUint32(REGISTER_INTERVAL, DEFAULT_UPDATE_INTERVAL);

    // ***
    // *** Initialize the reading index to 0.
    // ***
    writeUint32(REGISTER_READING_ID, 0);

    // ***
    // *** The start delay is the amount of time to wait
    // *** after power-up to take a reading from the sensor.
    // *** The data-sheets for both the DHT11 and the DHT22
    // *** state this time should be 1 second.
    // ***
    writeUint32(REGISTER_START_DELAY, 1000);

    // ***
    // *** Initialize the configuration register. This could be
    // *** done by writing a single byte, but it is easier to
    // *** tweak this way.
    // ***
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_SENSOR_ENABLED, 1);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_THRESHOLD_ENABLED, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_TRIGGER_READING, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_RESERVED_1, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_RESERVED_2, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_RESERVED_3, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_WRITE_CONFIG, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_RESET_CONFIG, 0);

    // ***
    // *** Initialize the thresholds.
    // ***
    writeFloat(REGISTER_LOWER_THRESHOLD, 21.0);
    writeFloat(REGISTER_UPPER_THRESHOLD, 26.0);

    // ***
    // *** Clear status register.
    // ***
    writeFloat(REGISTER_STATUS, 0);
  }

  // ***
  // *** Set the version.
  // ***
  _registers[REGISTER_VER_MAJOR] = VERSION_MAJOR;
  _registers[REGISTER_VER_MINOR] = VERSION_MINOR;
  _registers[REGISTER_VER_BUILD] = VERSION_BUILD;

  // ***
  // *** Set the device ID.
  // ***
  _registers[REGISTER_ID] = 0x2D;

  // ***
  // *** Display the configurable values.
  // ***
  displayConfiguration();

  // ***
  // *** The ground of the pin is supplied
  // *** by the pin DHT_POWER_PIN. This
  // *** allows the DHT to be turned off
  // *** over I2C to conserve power.
  // ***
  pinMode(DHT_POWER_PIN, OUTPUT);
  digitalWrite(DHT_POWER_PIN, LOW);
  setRegisterBit(REGISTER_STATUS, STATUS_SENSOR_IS_ENABLED, 1);

  // ***
  // *** Setup the DHT data pin.
  // ***
  pinMode(DHT_READING_PIN, INPUT);

  // ***
  // *** Set the interrupt pin for output.
  // ***
  pinMode(INTERRUPT_PIN, OUTPUT);
  digitalWrite(INTERRUPT_PIN, LOW);

  // ***
  // *** Setup the I2C bus.
  // ***
  byte deviceAddress = getDeviceAddress();

  // ***
  // *** Put the current device address into the register.
  // ***
  _registers[REGISTER_DEVICE_ADDRESS] = deviceAddress;
  displayDeviceAddress();

  Wire.begin(deviceAddress);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop()
{
  // ***
  // *** Check the I2C bus.
  // ***
  WireLoopCheck;

  // ***
  // *** Check the sensor interval to determine if
  // *** is it time to take another sensor reading.
  // ***
  checkSensorInterval();

  // ***
  // *** Check to see if a manual read was
  // *** triggered. Manual reads can be triggered
  // *** when the interval is set to 0.
  // ***
  checkForManualSensorRead();

  // ***
  // *** Check the thresholds.
  // ***
  checkThresholds();

  // ***
  // *** Check for configuration changes
  // ***
  checkForSensorEnabledChange();
  checkForResetConfiguration();
  checkForWriteConfiguration();
  checkForDeviceAddressChange();
}

void receiveEvent(uint8_t byteCount)
{
  // ***
  // *** Read all of the bytes from the wire.
  // ***
  byte buffer[byteCount];
  for (int i = 0; i < byteCount; i++)
  {
    buffer[i] = WireRead;
  }

  // ***
  // *** Process the data.
  // ***
  if (byteCount > 0)
  {
    // ***
    // *** The first byte sent is always the
    // *** register position.
    // ***
    uint8_t registerPosition = buffer[0];

    // ***
    // *** Ensure the register position is within bounds.
    // ***
    if (isStartableRegisterPosition(registerPosition))
    {
      // ***
      // *** Update the global register position
      // ***
      _registerPosition = registerPosition;

      // ***
      // *** Determine if this is a write operation or a read operation.
      // ***
      bool isWriteOperation = (byteCount > 1);

      // ***
      // *** Determine if there are bytes to write.
      // ***
      if (isWriteOperation)
      {
        // ***
        // *** Get the expected write count for this register position.
        // ***
        uint8_t expectedWriteCount = _registerSize[_registerPosition];

        if ((byteCount - 1) == expectedWriteCount && isWriteableRegisterPosition(_registerPosition))
        {
          // ***
          // *** Read the remaining bytes and write them to the registers.
          // ***
          for (uint8_t i = 1; i < byteCount; i++)
          {
            // ***
            // *** Read the next byte from the wire.
            // ***
            _registers[_registerPosition] = buffer[i];
            advanceRegisterPosition();
          }

          // ***
          // *** Set the write error status bit to success.
          // ***
          setRegisterBit(REGISTER_STATUS, STATUS_WRITE_ERROR, 0);
        }
        else
        {
          // ***
          // *** Set the write error status bit to failure.
          // ***
          setRegisterBit(REGISTER_STATUS, STATUS_WRITE_ERROR, 1);
        }

        // ***
        // *** Reset the request count to 0.
        // ***
        _requestCount = 0;

        // ***
        // *** Set the read error status bits.
        // ***
        setRegisterBit(REGISTER_STATUS, STATUS_READ_ERROR, 0);
      }
      else
      {
        // ***
        // *** Set the number of bytes to read in the request
        // ***
        _requestCount = _registerSize[_registerPosition];

        // ***
        // *** Set the read/write error status bits.
        // ***
        setRegisterBit(REGISTER_STATUS, STATUS_READ_ERROR, 0);
        setRegisterBit(REGISTER_STATUS, STATUS_WRITE_ERROR, 0);
      }
    }
    else
    {
      // ***
      // *** Set the read/write error status bits.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_READ_ERROR, byteCount == 1);
      setRegisterBit(REGISTER_STATUS, STATUS_WRITE_ERROR, byteCount > 1);
    }
  }
  else
  {
    // ***
    // *** Set the read/write error status bits.
    // ***
    setRegisterBit(REGISTER_STATUS, STATUS_READ_ERROR, 1);
    setRegisterBit(REGISTER_STATUS, STATUS_WRITE_ERROR, 0);
  }
}

void clearBuffer()
{
  // ***
  // *** Clear the receive buffer.
  // ***
  while (Wire.available())
  {
    WireRead;
  }
}

bool isStartableRegisterPosition(uint8_t registerPosition)
{
  return registerPosition >= 0 &&
         registerPosition < REGISTER_TOTAL_SIZE &&
         (_registerProtection[registerPosition] == 0 || _registerProtection[registerPosition] == 2);
}

bool isWriteableRegisterPosition(uint8_t registerPosition)
{
  return registerPosition >= 0 &&
         registerPosition < REGISTER_TOTAL_SIZE &&
         (_registerProtection[registerPosition] == 0);
}

void requestEvent()
{
  // ***
  // *** Send the next byte in the registers.
  // ***
  for (uint8_t i = 0; i < _requestCount; i++)
  {
    WireSend(_registers[_registerPosition]);
    advanceRegisterPosition();
  }

  // ***
  // *** Reset the request count to 0.
  // ***
  _requestCount = 0;
}

void checkSensorInterval()
{
  // ***
  // *** Check if the interval is enabled (0 = disabled or manual mode).
  // ***
  uint32_t interval = readUint32(REGISTER_INTERVAL);

  if (interval > 0)
  {
    // ***
    // *** Check the interval. Read the sensor once
    // *** every interval period.
    // ***
    if ((millis() - _lastReading) > interval)
    {
      readSensor();
    }
  }
}

void checkForManualSensorRead()
{
  // ***
  // *** Check if the interval is enabled (0 = disabled or manual mode).
  // ***
  uint32_t interval = readUint32(REGISTER_INTERVAL);

  // ***
  // *** We are in manual trigger mode. Check
  // *** if the trigger reading bit is set.
  // ***
  bool manualReadTriggered = getRegisterBit(REGISTER_CONFIG, CONFIG_BIT_TRIGGER_READING);

  if (interval == 0 && manualReadTriggered)
  {
    // ***
    // *** Read the sensor.
    // ***
    readSensor();
  }

  // ***
  // *** Reset the trigger reading bit.
  // ***
  setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_TRIGGER_READING, 0);
}

void readSensor()
{
  // ***
  // *** Only attempt a reading if the sensor is enabled.
  // ***
  bool sensorIsEnabled = getRegisterBit(REGISTER_STATUS, STATUS_SENSOR_IS_ENABLED);

  if (sensorIsEnabled)
  {
    // ***
    // *** This index tracks the number of readings taken. This gives
    // *** the master device the ability to "know" if the reading has
    // *** been updated since the last time it checked.
    // ***
    static uint32_t index = 0;

    // ***
    // *** Update the temperature and humidity registers.
    // ***
    uint8_t chk = _dht.read22(DHT_READING_PIN);

    // ***
    // *** Check for a valid read from the sensor.
    // ***
    if (chk == DHTLIB_OK)
    {
      // ***
      // *** Write the temperature to the register buffers.
      // ***
      writeFloat(REGISTER_TEMPERATURE, _dht.temperature);

      // ***
      // *** Write the humidity to the register buffers.
      // ***
      writeFloat(REGISTER_HUMIDITY, _dht.humidity);

      // ***
      // *** Update the reading index.
      // ***
      index++;
      writeUint32(REGISTER_READING_ID, index);

      // ***
      // *** Mark the last update.
      // ***
      _lastReading = millis();
    }
  }
}

void checkForSensorEnabledChange()
{
  // ***
  // *** Check the configuration to see if the sensor enabled
  // *** state was changed.
  // ***
  bool currentSensorIsEnabled = getRegisterBit(REGISTER_STATUS, STATUS_SENSOR_IS_ENABLED);
  bool expectedSensorIsEnabled = getRegisterBit(REGISTER_CONFIG, CONFIG_BIT_SENSOR_ENABLED);

  if (expectedSensorIsEnabled)
  {
    // ***
    // *** Make sure the sensor is enabled. Setting
    // *** the output of the digital port to LOW
    // *** turns the sensor on since the GND pin
    // *** of the sensor is connected to the digital pin.
    // ***
    if (!currentSensorIsEnabled)
    {
      // ***
      // *** The sensor is currently off and
      // *** needs to be turned on.
      // ***
      digitalWrite(DHT_POWER_PIN, LOW);

      // ***
      // *** Update the status register to indicate that
      // *** the sensor is enabled.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_SENSOR_IS_ENABLED, 1);

      // ***
      // *** Before attempting a reading, wait for
      // *** the sensor to stabilize.
      // ***
      WireDelay(readUint32(REGISTER_START_DELAY));
    }
  }
  else
  {
    if (currentSensorIsEnabled)
    {
      // ***
      // *** The sensor is currently on and
      // *** should be turned off.
      // ***
      digitalWrite(DHT_POWER_PIN, HIGH);

      // ***
      // *** Update the status register to indicate that
      // *** the sensor is disabled.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_SENSOR_IS_ENABLED, 0);
    }
  }
}

void checkThresholds()
{
  // ***
  // *** Check if thresholds are enabled. Bit 1 of the configuration
  // *** will be set to 1 (1 - enabled, 0 = disabled).
  // ***
  bool thresholdsEnabled =  getRegisterBit(REGISTER_CONFIG, CONFIG_BIT_THRESHOLD_ENABLED);

  if (thresholdsEnabled)
  {
    // ***
    // *** Set the enabled flag.
    // ***
    _thresholdsEnabled = true;

    // ***
    // *** Get the current temperature.
    // ***
    float currentTemperature = readFloat(REGISTER_TEMPERATURE);
    float lowerThreshold = readFloat(REGISTER_LOWER_THRESHOLD);
    float upperThreshold = readFloat(REGISTER_UPPER_THRESHOLD);

    // ***
    // *** Check if it is under or over either threshold.
    // ***
    if (currentTemperature <= lowerThreshold)
    {
      // ***
      // *** Set the interrupt pin.
      // ***
      digitalWrite(INTERRUPT_PIN, HIGH);

      // ***
      // *** Set/reset the appropriate status bits.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_UPPER_THRESHOLD_EXCEEDED, 0);
      setRegisterBit(REGISTER_STATUS, STATUS_LOWER_THRESHOLD_EXCEEDED, 1);
    }
    else if (currentTemperature >= upperThreshold)
    {
      // ***
      // *** Set the interrupt pin.
      // ***
      digitalWrite(INTERRUPT_PIN, HIGH);

      // ***
      // *** Set/reset the appropriate status bits.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_UPPER_THRESHOLD_EXCEEDED, 1);
      setRegisterBit(REGISTER_STATUS, STATUS_LOWER_THRESHOLD_EXCEEDED, 0);
    }
    else
    {
      // ***
      // *** Reset the interrupt pin.
      // ***
      digitalWrite(INTERRUPT_PIN, LOW);

      // ***
      // *** Reset the status bits.
      // ***
      setRegisterBit(REGISTER_STATUS, STATUS_UPPER_THRESHOLD_EXCEEDED, 0);
      setRegisterBit(REGISTER_STATUS, STATUS_LOWER_THRESHOLD_EXCEEDED, 0);
    }
  }
  else
  {
    // ***
    // *** Reset the flag.
    // ***
    _thresholdsEnabled = false;

    // ***
    // *** Ensure the interrupt pin is reset.
    // ***
    digitalWrite(INTERRUPT_PIN, LOW);

    // ***
    // *** Reset the status bits.
    // ***
    setRegisterBit(REGISTER_STATUS, STATUS_UPPER_THRESHOLD_EXCEEDED, 0);
    setRegisterBit(REGISTER_STATUS, STATUS_LOWER_THRESHOLD_EXCEEDED, 0);
  }
}

void checkForResetConfiguration()
{
  // ***
  // *** Check if the reset flag has been set and if yes,
  // *** reset the save settings.
  // ***
  bool resetConfig = getRegisterBit(REGISTER_CONFIG, CONFIG_BIT_RESET_CONFIG);

  if (resetConfig)
  {
    // ***
    // *** Reset the configuration.
    // ***
    resetConfiguration();

    // ***
    // *** Reset the configuration bit.
    // ***
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_RESET_CONFIG, 0);

    // ***
    // *** Reset the status bit.
    // ***
    setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED , 0);
  }
}

void checkForWriteConfiguration()
{
  // ***
  // *** Check if the reset flag has been set and if yes,
  // *** reset the save settings.
  // ***
  bool writeConfig = getRegisterBit(REGISTER_CONFIG, CONFIG_BIT_WRITE_CONFIG);

  if (writeConfig)
  {
    // ***
    // *** Save the configuration to EEPROM.
    // ***
    saveConfiguration();

    // ***
    // *** Reset the configuration bit.
    // ***
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_WRITE_CONFIG, 0);

    // ***
    // *** Set the status bit.
    // ***
    setRegisterBit(REGISTER_STATUS, STATUS_CONFIG_SAVED , 1);
  }
}

void checkForDeviceAddressChange()
{
  byte currentDeviceAddress = getDeviceAddress();
  byte newDeviceAddress = _registers[REGISTER_DEVICE_ADDRESS];

  if (newDeviceAddress != currentDeviceAddress)
  {
    setDeviceAddress(newDeviceAddress);
  }
}
