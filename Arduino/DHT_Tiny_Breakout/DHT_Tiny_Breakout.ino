#include <EEPROM.h>
#include <TinyWireS.h>
#include "dht.h"
#include "ByteConverter.h"
#include "Registers.h"
#include "Register_Defs.h"
#include "Configuration.h"

// ***
// *** Default slave address.
// ***
#define I2C_SLAVE_ADDRESS 0x26

// ***
// *** The pin on which the DHT Data pin is connected.
// ***
#define DHT_READING_PIN 3

// ***
// *** The pin on which the DHT GND pin is connected.
// ***
#define DHT_POWER_PIN 1

// ***
// *** Interrupt pin.
// ***
#define INTERRUPT_PIN 4

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
// *** Thenumber of bytes to return in the next request.
// ***
volatile uint8_t _requestCount = 0;

void setup()
{
  // ***
  // *** Restore the configuration from EEPROM. If the configuration
  // *** could not be restord, then se the default values.
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
    // *** after power up t take a reading from the sensor.
    // *** The data-sheets for both the DHT11 and the DHT22
    // *** state this time should be 1 second.
    // ***
    writeUint32(REGISTER_START_DELAY, 1000);

    // ***
    // *** Initialize the configuration register.
    // ***
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_SENSOR_ENABLED, 1);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_THRESHOLD_ENABLED, 0);
    setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_TRIGGER_READING, 0);

    // ***
    // *** Initialize the thresholds to 0.
    // ***
    writeFloat(REGISTER_UPPER_THRESHOLD, 0);
    writeFloat(REGISTER_LOWER_THRESHOLD, 0);

    // ***
    // *** Clear status register.
    // ***
    writeFloat(REGISTER_STATUS, 0);
  }

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
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent);
}

void loop()
{
  // ***
  // *** Check the I2C bus.
  // ***
  TinyWireS_stop_check();

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
}

void receiveEvent(uint8_t byteCount)
{
  if (byteCount > 0)
  {
    // ***
    // *** The first byte sent is always the
    // *** register position.
    // ***
    uint8_t registerPosition = TinyWireS.receive();

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
        // *** Read the remaining bytes and write them to the registers.
        // ***
        for (int i = 0; i < byteCount - 1; i++)
        {
          // ***
          // *** Read the next byte from the wire.
          // ***
          uint8_t value = TinyWireS.receive();

          // ***
          // *** Don't allow a read-only register position
          // *** to be written to.
          // ***
          if (isWriteableRegisterPosition(_registerPosition))
          {
            _registers[_registerPosition] = value;
          }

          advanceRegisterPosition();
        }

        // ***
        // *** Reset the request count to 0.
        // ***
        _requestCount = 0;

        // ***
        // *** Set the read/write error status bits.
        // ***
        setRegisterBit(REGISTER_STATUS, STATUS_READ_ERROR, 0);
        setRegisterBit(REGISTER_STATUS, STATUS_WRITE_ERROR, 0);
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
  while (TinyWireS.available())
  {
    TinyWireS.receive();
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
  for (int i = 0; i < _requestCount; i++)
  {
    TinyWireS.send(_registers[_registerPosition]);
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

  if (interval == 0)
  {
    // ***
    // *** We are in manual trigger mode. Check
    // *** if the trigger reading bit is set.
    // ***
    bool manualReadTriggered = getRegisterBit(REGISTER_CONFIG, CONFIG_BIT_TRIGGER_READING);

    if (manualReadTriggered)
    {
      // ***
      // *** Reset the trigger reading bit.
      // ***
      setRegisterBit(REGISTER_CONFIG, CONFIG_BIT_TRIGGER_READING, 0);

      // ***
      // *** Read the sensor.
      // ***
      readSensor();
    }
  }
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
      tws_delay(readUint32(REGISTER_START_DELAY));
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
  else if (_thresholdsEnabled)
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
