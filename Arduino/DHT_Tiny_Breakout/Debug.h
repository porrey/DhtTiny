#if defined( __AVR_ATtiny85__ )
void initDebug() {}
void displayRegisters() {}
void displaySavedConfiguration() {}
#else
void initDebug()
{
  Serial.begin(115200);
  delay(500);

  Serial.println("DHT Tiny");
  Serial.print("Firmware Version: "); Serial.print(VERSION_MAJOR); Serial.print("."); Serial.print(VERSION_MINOR); Serial.print(" (Build "); Serial.print(VERSION_BUILD); Serial.println(")");
  Serial.println("");
}

void displayRegisters()
{

  Serial.println("C# Code:");
  Serial.print("private const byte REGISTER_READALL = "); Serial.print(REGISTER_READALL); Serial.println(";");
  Serial.print("private const byte REGISTER_TEMPERATURE = "); Serial.print(REGISTER_TEMPERATURE); Serial.println(";");
  Serial.print("private const byte REGISTER_HUMIDITY = "); Serial.print(REGISTER_HUMIDITY); Serial.println(";");
  Serial.print("private const byte REGISTER_READING_ID = "); Serial.print(REGISTER_READING_ID); Serial.println(";");
  Serial.print("private const byte REGISTER_INTERVAL = "); Serial.print(REGISTER_INTERVAL); Serial.println(";");
  Serial.print("private const byte REGISTER_UPPER_THRESHOLD = "); Serial.print(REGISTER_UPPER_THRESHOLD); Serial.println(";");
  Serial.print("private const byte REGISTER_LOWER_THRESHOLD = "); Serial.print(REGISTER_LOWER_THRESHOLD); Serial.println(";");
  Serial.print("private const byte REGISTER_START_DELAY = "); Serial.print(REGISTER_START_DELAY); Serial.println(";");
  Serial.print("private const byte REGISTER_CONFIG = "); Serial.print(REGISTER_CONFIG); Serial.println(";");
  Serial.print("private const byte REGISTER_STATUS = "); Serial.print(REGISTER_STATUS); Serial.println(";");
  Serial.print("private const byte REGISTER_VER_MAJOR = "); Serial.print(REGISTER_VER_MAJOR); Serial.println(";");
  Serial.print("private const byte REGISTER_VER_MINOR = "); Serial.print(REGISTER_VER_MINOR); Serial.println(";");
  Serial.print("private const byte REGISTER_VER_BUILD = "); Serial.print(REGISTER_VER_BUILD); Serial.println(";");
  Serial.print("private const byte REGISTER_TOTAL_SIZE = "); Serial.print(REGISTER_TOTAL_SIZE); Serial.println(";");
}

void displaySavedConfiguration()
{
  Serial.println("");
  Serial.println("Saved Values:");
  Serial.print("Interval = "); Serial.println(readUint32(REGISTER_INTERVAL));
  Serial.print("Upper Threshold = "); Serial.println(readFloat(REGISTER_UPPER_THRESHOLD));
  Serial.print("Lower Threshold = "); Serial.println(readFloat(REGISTER_LOWER_THRESHOLD));
  Serial.print("Start Delay = "); Serial.println(readUint32(REGISTER_START_DELAY));
  Serial.print("Configuration = "); Serial.println(_registers[REGISTER_CONFIG]);
}
#endif
