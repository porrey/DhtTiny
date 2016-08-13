namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public static class Bit
	{
		public static bool Get(byte value, byte bitIndex)
		{
			return ((value & (1 << bitIndex)) == 1);
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
