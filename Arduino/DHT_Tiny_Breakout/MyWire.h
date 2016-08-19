#ifndef MY_WIRE_H
#define MY_WIRE_H

#if defined( __AVR_ATtiny85__ )

// ***
// *** Use the Tiny Wire library.
// ***
#include <TinyWireS.h>
USI_TWI_S Wire;

#define WireLoopCheck   TinyWireS_stop_check();
#define WireRead        Wire.receive()
#define WireSend(a)     Wire.send(a)
#define WireDelay(a)    tws_delay(a);

#else

// ***
// *** Use the standard Wire library.
// ***
#include <Wire.h>

#define WireLoopCheck   void
#define WireRead        Wire.read()
#define WireSend(a)     Wire.write(a)
#define WireDelay(a)    delay(a);

#endif
#endif
