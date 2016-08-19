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