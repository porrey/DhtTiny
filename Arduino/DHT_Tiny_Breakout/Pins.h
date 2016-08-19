#ifndef PINS_H
#define PINS_H

#if defined( __AVR_ATtiny85__ )

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

#else

// ***
// *** The pin on which the DHT Data pin is connected.
// ***
#define DHT_READING_PIN 3

// ***
// *** The pin on which the DHT GND pin is connected.
// ***
#define DHT_POWER_PIN 2

// ***
// *** Interrupt pin.
// ***
#define INTERRUPT_PIN 4

#endif
#endif
