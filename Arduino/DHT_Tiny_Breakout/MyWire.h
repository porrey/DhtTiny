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
