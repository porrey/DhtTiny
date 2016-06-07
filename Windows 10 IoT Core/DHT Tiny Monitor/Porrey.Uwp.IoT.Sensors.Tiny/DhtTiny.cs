using Windows.Devices.I2c;

namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public class DhtTiny : I2c
	{
		private const byte REGISTER_READALL = 0;
		private const byte REGISTER_TEMPERATURE = 4;
		private const byte REGISTER_HUMIDITY = 8;
		private const byte REGISTER_INTERVAL = 12;
		private const byte REGISTER_READING_ID = 16;
		private const byte REGISTER_UPPER_THRESHOLD = 20;
		private const byte REGISTER_LOWER_THRESHOLD = 24;
		private const byte REGISTER_START_DELAY = 28;
		private const byte REGISTER_CONFIG = 29;
		private const byte REGISTER_STATUS = 30;

		private const byte REGISTER_SIZE = 31;

		public DhtTiny(byte deviceAddress)
			: base(deviceAddress)
		{
		}

		public DhtTiny(byte deviceAddress, I2cBusSpeed busSpeed)
			: base(deviceAddress, busSpeed)
		{
		}

		private int _interval = 0;
		public int Interval
		{
			get
			{
				return _interval;
			}
			set
			{
				_interval = value;
			}
		}

		public bool Refresh()
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** Set the register address to 0.
				// ***
				this.Device.Write(new byte[] { 0 });

				// ***
				// *** Read all of the registers.
				// ***
				byte[] readBuffer = new byte[REGISTER_SIZE];

				for (int i = 0; i < REGISTER_SIZE; i++)
				{
					byte[] localBuffer = new byte[1];
					this.Device.Read(localBuffer);
					readBuffer[i] = localBuffer[0];
				}
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool GetInterval(int interval)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_INTERVAL };

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4];
				this.Device.WriteRead(writeBuffer, readBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool SetInterval(int interval)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[5] { REGISTER_INTERVAL, 0, 0, 0, 0 };

				// ***
				// *** Write the register value and the value.
				// ***
				this.Device.Write(writeBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}
	}
}
