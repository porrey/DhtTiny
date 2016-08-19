using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public delegate void FindAllDhtTinyCallbackDelegate(I2cScanEventArgs e);

	public class DhtTiny : I2c
	{
		private const byte REGISTER_ID = 0;
		private const byte REGISTER_VER_MAJOR = 1;
		private const byte REGISTER_VER_MINOR = 2;
		private const byte REGISTER_VER_BUILD = 3;
		private const byte REGISTER_TEMPERATURE = 4;
		private const byte REGISTER_HUMIDITY = 8;
		private const byte REGISTER_STATUS = 12;
		private const byte REGISTER_READING_ID = 13;
		private const byte REGISTER_INTERVAL = 17;
		private const byte REGISTER_UPPER_THRESHOLD = 21;
		private const byte REGISTER_LOWER_THRESHOLD = 25;
		private const byte REGISTER_START_DELAY = 29;
		private const byte REGISTER_CONFIG = 33;
		private const byte REGISTER_DEVICE_ADDRESS = 34;

		private const byte REGISTER_TOTAL_SIZE = 35;

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

		public static async Task<IEnumerable<byte>> FindAllDhtTinyAsync(FindAllDhtTinyCallbackDelegate callback = null)
		{
			IList<byte> returnValue = new List<byte>();

			// ***
			// *** Get a selector string that will return all I2C controllers on the system
			// ***
			string aqs = I2cDevice.GetDeviceSelector();

			// ***
			// *** Find the I2C bus controller device with our selector string
			// ***
			var dis = await DeviceInformation.FindAllAsync(aqs).AsTask();

			if (dis.Count > 0)
			{
				const int minimumAddress = 8;
				const int maximumAddress = 77;

				for (byte address = minimumAddress; address <= maximumAddress; address++)
				{
					// ***
					// *** Invoke the callback for progress upates.
					// ***
					if (callback != null)
					{
						callback.Invoke(new I2cScanEventArgs(address, address - minimumAddress, maximumAddress - minimumAddress, returnValue));
					}

					var settings = new I2cConnectionSettings(address);
					settings.BusSpeed = I2cBusSpeed.FastMode;
					settings.SharingMode = I2cSharingMode.Shared;

					// ***
					// *** Create an I2cDevice with our selected bus controller and I2C settings
					// ***
					using (I2cDevice device = await I2cDevice.FromIdAsync(dis[0].Id, settings))
					{
						if (device != null)
						{
							try
							{
								byte[] writeBuffer = new byte[1] { REGISTER_ID };
								device.Write(writeBuffer);
								byte[] readBuffer = new byte[1];
								device.Read(readBuffer);

								if (readBuffer[0] == 0x2D)
								{
									returnValue.Add(address);
								}
							}
							catch
							{
								// ***
								// *** If the address is invalid, an exception will be thrown.
								// ***
							}
						}
					}
				}
			}

			return returnValue;
		}

		public bool GetStatusBit(byte status, DhtTiny.StatusBit bitIndex)
		{
			return Bit.Get(status, (byte)bitIndex);
		}

		public bool GetConfigurationBit(byte configuration, DhtTiny.ConfigBit bitIndex)
		{
			return Bit.Get(configuration, (byte)bitIndex);
		}

		public async Task<bool> GetIsEnabledAsync()
		{
			return ((await this.GetConfigurationAsync() & (1 << (byte)ConfigBit.SensorEnabled)) == 1) ? true : false;
		}

		public async Task<bool> SetIsEnabledAsync(bool enable)
		{
			byte config = Bit.Set(await this.GetConfigurationAsync(), (byte)ConfigBit.SensorEnabled, enable);
			return await this.SetConfigurationAsync(config);
		}

		public async Task<bool> GetThresholdsAreEnabledAsync()
		{
			return ((await this.GetConfigurationAsync() & (1 << (byte)ConfigBit.ThresholdEnabled)) == 1) ? true : false;
		}

		public async Task<bool> SetThresholdsAreEnabled(bool enable)
		{
			byte config = Bit.Set(await this.GetConfigurationAsync(), (byte)ConfigBit.ThresholdEnabled, enable);
			return await this.SetConfigurationAsync(config);
		}

		public async Task<float> GetTemperatureAsync()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_TEMPERATURE };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<float> GetHumidityAsync()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_HUMIDITY };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<uint> GetIntervalAsync()
		{
			uint returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_INTERVAL };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToUInt32(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> SetIntervalAsync(uint value)
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
				returnValue = await this.WriteAsync(writeBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<uint> GetStartDelayAsync()
		{
			uint returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_START_DELAY };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToUInt32(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> SetStartDelayAsync(uint value)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] data = BitConverter.GetBytes(value);
				byte[] writeBuffer = new byte[5] { REGISTER_START_DELAY, data[0], data[1], data[2], data[3] };

				// ***
				// *** Write the register value and the value.
				// ***
				returnValue = await this.WriteAsync(writeBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<uint> GetReadingIdAsync()
		{
			uint returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_READING_ID };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToUInt32(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<float> GetUpperThresholdAsync()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_UPPER_THRESHOLD };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> SetUpperThresholdAsync(float value)
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
				await this.WriteAsync(writeBuffer);
				returnValue = (await this.GetUpperThresholdAsync() == value);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<float> GetLowerThresholdAsync()
		{
			float returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_LOWER_THRESHOLD };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[4] { 0, 0, 0, 0 };
				await this.ReadAsync(readBuffer);
				returnValue = BitConverter.ToSingle(readBuffer, 0);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> SetLowerThresholdAsync(float value)
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
				// *** Read from the device.
				// ***
				returnValue = await this.WriteAsync(writeBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetStatusAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_STATUS };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetConfigurationAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_CONFIG };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> GetConfigurationAsync(byte position)
		{
			return Bit.Get(await this.GetConfigurationAsync(), position);
		}

		public async Task<bool> SetConfigurationAsync(byte value)
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
				returnValue = await this.WriteAsync(writeBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> SetConfigurationAsync(byte position, bool bit)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** Get the current configuration value.
				// ***
				byte currentValue = await this.GetConfigurationAsync();

				// ***
				// *** Set the bit.
				// ***
				currentValue = Bit.Set(currentValue, position, bit);

				// ***
				// *** Write the new value.
				// ***
				returnValue = await this.SetConfigurationAsync(currentValue);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetDeviceIdAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_ID };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetDeviceAddressAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_DEVICE_ADDRESS };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<bool> SetDeviceAddressAsync(byte value)
		{
			bool returnValue = false;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[2] { REGISTER_DEVICE_ADDRESS, value };

				// ***
				// *** Write the register value and the value.
				// ***
				returnValue = await this.WriteAsync(writeBuffer);
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetFirmwareMajorVersionAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_VER_MAJOR };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetFirmwareMinorVersionAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_VER_MINOR };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<byte> GetFirmwareBuildVersionAsync()
		{
			byte returnValue = 0;

			if (this.IsInitialized)
			{
				// ***
				// *** The register ID
				// ***
				byte[] writeBuffer = new byte[1] { REGISTER_VER_BUILD };
				await this.WriteAsync(writeBuffer);

				// ***
				// *** Read from the device.
				// ***
				byte[] readBuffer = new byte[1] { 0 };
				await this.ReadAsync(readBuffer);
				returnValue = readBuffer[0];
			}
			else
			{
				throw new DeviceNotInitializedException();
			}

			return returnValue;
		}

		public async Task<string> GetFirmwareVersionAsync()
		{
			return string.Format("{0}.{1} (Build {2})",
						(await this.GetFirmwareMajorVersionAsync()).ToString(),
						(await this.GetFirmwareMinorVersionAsync()).ToString(),
						(await this.GetFirmwareBuildVersionAsync()).ToString());
		}
	}
}
