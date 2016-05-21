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
#define DHT_I2C_ADDR  0x26

// ***
// *** Address of each variable
// *** within the registers.
// ***
#define REGISTER_TEMPERATURE             0
#define REGISTER_HUMIDITY                4
#define REGISTER_INTERVAL                8
#define REGISTER_READING_ID             12
#define REGISTER_UPPER_THRESHOLD        16
#define REGISTER_LOWER_THRESHOLD        20
#define REGISTER_START_DELAY            24
#define REGISTER_CONFIG                 28
#define REGISTER_STATUS                 30
#define REGISTER_TOTAL_SIZE             REGISTER_STATUS + SIZE_UINT8

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

#define DEVICE_DELAY 50

// ***
// ***
// ***
uint32_t _myInterval = 2000;

void setup()
{
  // ***
  // *** Initialize the Serial.
  // ***
  Serial.begin(9600);

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
  // *** Iniialize the I2C.
  // ***
  Wire.begin();
  Wire.setClock(400000);
}

// ***
// *** Define the testing states/intervals.
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

        Serial.println(F("******************************"));
        Serial.println(F("Setting interval to 2 seconds, checking device every 2 seconds."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Disabling the sensor."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Enabling the sensor."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Enabling thresholds."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Disabling thresholds."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Setting interval to 1 second, checking device every 1 second."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Setting interval to 0, checking device every 2 seconds."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Triggering a reading manually."));
        Serial.println(F("******************************"));
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
        Serial.println(F("******************************"));
        Serial.println(F("Setting interval to 2 seconds, checking device every 2 seconds."));
        Serial.println(F("******************************"));
        Serial.println(F(""));
        sendUint32(REGISTER_INTERVAL, 2000);
        _myInterval = 2000;
        break;
      }
    case STATE_BAD_WRITE:
      {
        Serial.println(F("******************************"));
        Serial.println(F("Writing to wrong register position."));
        Serial.println(F("******************************"));
        Serial.println(F(""));

        // ***
        // *** The device will reject this update.
        // ***
        sendUint32(REGISTER_INTERVAL + 1, 1000);

        break;
      }
    case STATE_SAVE_CONFIG:
      {
        Serial.println(F("******************************"));
        Serial.println(F("Saving the configuration."));
        Serial.println(F("******************************"));
        Serial.println(F(""));

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

        // ***
        // *** Set the configuration bit to indicate we
        // *** want the currnt configuration saved to EEPROM.
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
  Serial.print("\tWaiting "); Serial.print(_myInterval / 1000); Serial.println(" second(s) until next request.");
  Serial.println("");
  delay(_myInterval);

  // ***
  // *** Increment the loop counter.
  // ***
  _myCounter++;
}

void rebootDevice()
{
  Serial.println(F("******************************"));
  Serial.println(F("Rebooting device."));
  Serial.println(F("******************************"));
  Serial.println(F(""));
  digitalWrite(DHT_RESET_PIN, LOW);
  delay(150);
  digitalWrite(DHT_RESET_PIN, HIGH);
  delay(2000);
}

void displayData()
{
  // ***
  // *** Read all of the registers at once for efficiency.
  // ***
  byte registers[REGISTER_TOTAL_SIZE];
  requestBytes(0, REGISTER_TOTAL_SIZE, registers);

  // ***
  // *** Get the current reading ID.
  // ***
  uint32_t id = ByteConverter::bytesToUint32(&registers[REGISTER_READING_ID]);
  float temperatureC = ByteConverter::bytesToFloat(&registers[REGISTER_TEMPERATURE]);
  float temperatureF = temperatureC * 1.8 + 32.0;
  float humidity = ByteConverter::bytesToFloat(&registers[REGISTER_HUMIDITY]);
  float upperThreshold = ByteConverter::bytesToFloat(&registers[REGISTER_UPPER_THRESHOLD]);
  float lowerThreshold = ByteConverter::bytesToFloat(&registers[REGISTER_LOWER_THRESHOLD]);
  uint8_t configValue = registers[REGISTER_CONFIG];
  uint8_t statusValue = registers[REGISTER_STATUS];

  Serial.print("Reading ID = "); Serial.println(id);
  Serial.print("\tInterval = "); Serial.println(requestUint32(REGISTER_INTERVAL));
  Serial.print("\tTemperature = "); Serial.print(temperatureC); Serial.print("C, "); Serial.print(temperatureF); Serial.println("F");
  Serial.print("\tHumidity = "); Serial.print(humidity); Serial.println("%");
  Serial.print("\tUpper Threshold = "); Serial.print(upperThreshold); Serial.println("C");
  Serial.print("\tLower Threshold = "); Serial.print(lowerThreshold); Serial.println("C");

  // ***
  // *** Display the configuration bits.
  // ***
  Serial.print("\tConfigurations bits: ");

  displayBits(configValue);
  Serial.println("");

  // ***
  // *** Display the status bits.
  // ***
  Serial.print("\tStatus bits:         ");
  displayBits(statusValue);
  Serial.println("");

  if (bitRead(configValue, CONFIG_BIT_THRESHOLD_ENABLED) == 1) Serial.println("\tThresholds are enabled."); else Serial.println("\tThresholds are disabled.");
  if (bitRead(statusValue, STATUS_UPPER_THRESHOLD_EXCEEDED) == 1) Serial.println("\tUpper threshold exceeded.");
  if (bitRead(statusValue, STATUS_LOWER_THRESHOLD_EXCEEDED) == 1) Serial.println("\tLower threshold exceeded.");

  if (bitRead(configValue, CONFIG_BIT_TRIGGER_READING) == 1) Serial.println("\tTrigger Manual Read bit is 1."); else Serial.println("\tTrigger Manual Read bit is 0.");
}

void displayBits(uint8_t byteValue)
{
  // ***
  // *** Display the bits with a '-'
  // *** character between each bit.
  // ***
  for (int i = 7; i >= 0; i--)
  {
    Serial.print(bitRead(byteValue, i)); if (i > 0) Serial.print("-");
  }
}

void requestBytes(uint8_t registerId, uint8_t byteCount, uint8_t* data)
{
  // ***
  // *** Send the byte array to the i2c device.
  // ***
  Wire.beginTransmission(DHT_I2C_ADDR);
  Wire.write(registerId);
  byte response = Wire.endTransmission(false);

  if (response == 0)
  {
    /*Wire.requestFrom(DHT_I2C_ADDR, byteCount);
      for (int i = 0; i < byteCount; i++)
      {
      data[i] = Wire.read();
      }*/

    for (int i = 0; i < byteCount; i++)
    {
      Wire.requestFrom(DHT_I2C_ADDR, 1);
      data[i] = Wire.read();
    }
  }
}

bool sendBytes(uint8_t registerId, uint8_t byteCount, uint8_t* data)
{
  bool returnValue = false;

  // ***
  // *** Send the byte array to the i2c device.
  // ***
  Wire.beginTransmission(DHT_I2C_ADDR);
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

uint8_t requestUint8(uint8_t registerId)
{
  float returnValue = 0;

  byte data[1];
  requestBytes(registerId, 1, data);
  returnValue = data[0];

  return returnValue;
}

bool sendUint8(uint8_t registerId, uint8_t value)
{
  bool returnValue = false;

  byte data[1] = { value };
  returnValue = sendBytes(registerId, 1, data);

  return returnValue;
}

float requestFloat(uint8_t registerId)
{
  float returnValue = 0;

  byte data[4];
  requestBytes(registerId, 4, data);
  returnValue = ByteConverter::bytesToFloat(data);

  return returnValue;
}

bool sendFloat(uint8_t registerId, float value)
{
  bool returnValue = false;

  byte data[4];
  ByteConverter::floatToBytes(value, data);
  returnValue = sendBytes(registerId, 4, data);

  return returnValue;
}

uint32_t requestUint32(uint8_t registerId)
{
  uint32_t returnValue = 0;

  byte data[4];
  requestBytes(registerId, 4, data);
  returnValue = ByteConverter::bytesToUint32(data);

  return returnValue;
}

bool sendUint32(uint8_t registerId, uint32_t value)
{
  bool returnValue = false;

  byte data[4];
  ByteConverter::uint32ToBytes(value, data);
  returnValue = sendBytes(registerId, 4, data);

  return returnValue;
}

uint8_t calculateCrc8(uint8_t *data, byte byteCount)
{
  uint8_t crc = 0x00;

  while (byteCount--)
  {
    uint8_t extract = *data++;

    for (uint8_t tempI = 8; tempI; tempI--)
    {
      uint8_t sum = (crc ^ extract) & 0x01;
      crc >>= 1;

      if (sum)
      {
        crc ^= 0x8C;
      }

      extract >>= 1;
    }
  }

  return crc;
}
