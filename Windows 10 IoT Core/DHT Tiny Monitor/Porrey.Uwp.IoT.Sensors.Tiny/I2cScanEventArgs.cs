using System;
using System.Collections.Generic;

namespace Porrey.Uwp.IoT.Sensors.Tiny
{
	public class I2cScanEventArgs : EventArgs
	{
		public I2cScanEventArgs(byte currentAddress, int currentIndex, int total, IEnumerable<byte> items)
		{
			this.CurrentAddress = currentAddress;
			this.CurrentIndex = currentIndex;
			this.Total = total;
			this.Items = items;
		}

		public IEnumerable<byte> Items { get; private set; }
		public byte CurrentAddress { get; private set; }
		public int CurrentIndex { get; private set; }
		public int Total { get; private set; }
	}
}
