using System;
using Windows.Devices.I2c;

namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public class DhtTiny : I2c
	{
		private const byte REGISTER_READALL = 0;
		private const byte REGISTER_TEMPERATURE = 1;
		private const byte REGISTER_HUMIDITY = 5;
		private const byte REGISTER_INTERVAL = 9;
		private const byte REGISTER_READING_ID = 13;
		private const byte REGISTER_UPPER_THRESHOLD = 17;
		private const byte REGISTER_LOWER_THRESHOLD = 21;
		private const byte REGISTER_START_DELAY = 25;
		private const byte REGISTER_CONFIG = 29;
		private const byte REGISTER_STATUS = 30;
		private const byte REGISTER_VER_MAJOR = 31;
		private const byte REGISTER_VER_MINOR = 32;
		private const byte REGISTER_VER_BUILD = 33;
		private const byte REGISTER_TOTAL_SIZE = 34;

		// ***
		// *** Configuration bits.
		// ***
		public enum ConfigBit
		{
			SensorEnabled = 0,
			ThresholdEnabled = 1,
			TriggerReading = 2,
			Reserved1 = 3,
			Reserved2 = 4,
			Reserved3 = 5,
			WriteConfig = 6,
			ResetConfig = 7
		}

		// ***
		// *** Status register bits.
		// ***
		public enum StatusBit
		{
			IsEnabled = 0,
			UpperThresholdExceeded = 1,
			LowerThresholdExceeded = 2,
			Reserved1 = 3,
			Reserved2 = 4,
			ConfigSaved = 5,
			ReadError = 6,
			WriteError = 7,
		}

		public DhtTiny(byte deviceAddress)
				: base(deviceAddress, I2cBusSpeed.FastMode)
		{
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
				byte[] readBuffer = new byte[REGISTER_TOTAL_SIZE];
				this.Device.Read(readBuffer);

				this.Temperature = BitConverter.ToSingle(readBuffer, REGISTER_TEMPERATURE);
				this.Humidity = BitConverter.ToSingle(readBuffer, REGISTER_HUMIDITY);
				this.Interval = BitConverter.ToUInt32(readBuffer, REGISTER_INTERVAL);
				this.ReadingId = BitConverter.ToUInt32(readBuffer, REGISTER_READING_ID);
				this.UpperThreshold = BitConverter.ToSingle(readBuffer, REGISTER_UPPER_THRESHOLD);
				this.LowerThreshold = BitConverter.ToSingle(readBuffer, REGISTER_LOWER_THRESHOLD);
				this.StartDelay = BitConverter.ToUInt32(readBuffer, REGISTER_START_DELAY);
				this.Configuration = readBuffer[REGISTER_CONFIG];
				this.Status = readBuffer[REGISTER_STATUS];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		#region Cache Members
		// ***
		// *** These members access the cached values that
		// *** get updated with each call to Refresh();
		// ***
		public float Temperature { get; private set; }
		public float Humidity { get; private set; }
		public uint Interval { get; private set; }
		public uint ReadingId { get; private set; }
		public float UpperThreshold { get; private set; }
		public float LowerThreshold { get; private set; }
		public uint StartDelay { get; private set; }
		public byte Configuration { get; private set; }
		public byte Status { get; private set; }

		public bool GetStatusBit(DhtTiny.StatusBit bitIndex)
		{
			return Bit.Get(this.Status, (byte)bitIndex);
		}

		public bool GetConfigurationBit(DhtTiny.ConfigBit bitIndex)
		{
			return Bit.Get(this.Configuration, (byte)bitIndex);
		}

		public bool IsEnabled
		{
			get
			{
				return this.GetConfigurationBit(ConfigBit.SensorEnabled);
			}
		}

		public bool ThresholdsAreEnabled
		{
			get
			{
				return this.GetConfigurationBit(ConfigBit.ThresholdEnabled);
			}
		}
		#endregion

		#region Real-time Members
		// ***
		// *** These methods are dynamic - they communicate with the
		// *** device each time they are called.
		// ***
		public bool GetIsEnabled()
		{
			return ((this.GetConfiguration() & (1 << (byte)ConfigBit.SensorEnabled)) == 1) ? true : false;
		}

		public void SetIsEnabled(bool enable)
		{
			byte config = Bit.Set(this.GetConfiguration(), (byte)ConfigBit.SensorEnabled, enable);
			this.SetConfiguration(config);
		}

		public bool GetThresholdsAreEnabled()
		{
				return ((this.GetConfiguration() & (1 << (byte)ConfigBit.ThresholdEnabled)) == 1) ? true : false;
		}

		public void SetThresholdsAreEnabled(bool enable)
		{
				byte config = Bit.Set(this.GetConfiguration(), (byte)ConfigBit.ThresholdEnabled, enable);
				this.SetConfiguration(config);
		}

		public float GetTemperature()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_TEMPERATURE };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				this.Device.Read(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public float GetHumidity()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_HUMIDITY };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				this.Device.Read(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public uint GetInterval()
		{
			uint returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_INTERVAL };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				this.Device.Read(readBuffer);
				returnValue = BitConverter.ToUInt32(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool SetInterval(uint value)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] data = BitConverter.GetBytes(value);
				byte[] writeBuffer = new byte[5] { REGISTER_INTERVAL, data[0], data[1], data[2], data[3] };

				// ***
				// *** Write the register value and the value.
				// ***
				this.Device.Write(writeBuffer);
				returnValue = true;
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public uint GetReadingId()
		{
			uint returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_READING_ID };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				this.Device.Read(readBuffer);
				returnValue = BitConverter.ToUInt32(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public float GetUpperThreshold()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_UPPER_THRESHOLD };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				this.Device.Read(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool SetUpperThreshold(float value)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] data = BitConverter.GetBytes(value);
				byte[] writeBuffer = new byte[5] { REGISTER_UPPER_THRESHOLD, data[0], data[1], data[2], data[3] };

				// ***
				// *** Write the register value and the value.
				// ***
				this.Device.Write(writeBuffer);
				returnValue = (this.GetUpperThreshold() == value);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public float GetLowerThreshold()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_LOWER_THRESHOLD };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				this.Device.Read(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool SetLowerThreshold(float value)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] data = BitConverter.GetBytes(value);
				byte[] writeBuffer = new byte[5] { REGISTER_LOWER_THRESHOLD, data[0], data[1], data[2], data[3] };

				// ***
				// *** Write the register value and the value.
				// ***
				this.Device.Write(writeBuffer);
				returnValue = true;
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public byte GetStatus()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_STATUS };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				this.Device.Read(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public byte GetConfiguration()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_CONFIG };
				this.Device.Write(writeBuffer);

				// ***
				// *** Write the register value and read from the device
				// *** at the same time.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				this.Device.Read(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool GetConfiguration(byte position)
		{
			return Bit.Get(this.GetConfiguration(), position);
		}

		public bool SetConfiguration(byte value)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[2] { REGISTER_CONFIG, value };

				// ***
				// *** Write the register value and the value.
				// ***
				this.Device.Write(writeBuffer);

				returnValue = true;
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public bool SetConfiguration(byte position, bool bit)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** Get the current configuration value.
				// ***
				byte currentValue = this.GetConfiguration();

				// ***
				// *** Set the bit.
				// ***
				currentValue = Bit.Set(currentValue, position, bit);

				// ***
				// *** Write the new value.
				// ***
				returnValue = this.SetConfiguration(currentValue);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}
		#endregion
	}
}
