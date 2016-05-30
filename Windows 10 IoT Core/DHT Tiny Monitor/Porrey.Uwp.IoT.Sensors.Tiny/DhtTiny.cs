using Windows.Devices.I2c;

namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public class DhtTiny : I2c
	{
		public DhtTiny(byte deviceAddress) : base(deviceAddress)
		{
		}

		public DhtTiny(byte deviceAddress, I2cBusSpeed busSpeed) : base(deviceAddress, busSpeed)
		{
		}
	}
}
