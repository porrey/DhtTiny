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
using System.Linq;
using Windows.UI.Xaml.Data;

namespace Porrey.Tiny.Dht.Common
{
	public sealed class ByteToHexConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string language)
		{
			string returnValue = string.Empty;

			if (value != null)
			{
				if ((double)value >= 0 && (double)value <= 255)
				{
					returnValue = string.Format("0x{0:X2}", System.Convert.ToByte(value));
				}
				else
				{
					returnValue = value.ToString();
				}
			}

			return returnValue;
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			byte returnValue = 0;

			if (value is string)
			{
				string hex = (string)value;

				returnValue = Enumerable.Range(0, hex.Length)
										.Where(x => x % 2 == 0)
										.Select(x => System.Convert.ToByte(hex.Substring(x, 2), 16))
										.FirstOrDefault();
			}

			return returnValue;
		}
	}
}