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
using System;

namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public static class Bit
	{
		public static bool Get(byte value, byte bitIndex)
		{
			int mask = (1 << bitIndex);
			int maskedValue = (value & mask);
			return (maskedValue == mask);
		}

		public static byte Set(byte value, byte bitIndex, bool bit)
		{
			byte returnValue = value;

			if (bit)
			{
				returnValue = (byte)(returnValue | (1 << bitIndex));
			}
			else
			{
				returnValue = (byte)(returnValue & ~(1 << bitIndex));
			}

			return returnValue;
		}
	}
}
