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
using Porrey.Uwp.IoT.Sensors.Tiny;
using Windows.UI.Popups;
using Windows.UI.Xaml.Controls;

namespace DhtTinyMonitor.Views
{
	public sealed partial class Settings : ContentDialog
	{
		private DhtTiny _dhtTiny = null;
		private UInt32 _interval = 0;
		private float _lowerThreshold = 0f;
		private float _upperThreshold = 0f;
		private UInt32 _startDelay = 0;
		private byte _address = 0;
		private byte _model = 0;
		private ModelItem[] _models = new ModelItem[]
				{
					new ModelItem() { Index= 0, Model = 11},
					new ModelItem() { Index= 1, Model = 21},
					new ModelItem() { Index= 2, Model = 22},
					new ModelItem() { Index= 3, Model = 33},
					new ModelItem() { Index= 4, Model = 44},
				};

		private class ModelItem
		{
			public int Index { get; set; }
			public byte Model { get; set; }
		}

		public Settings(DhtTiny dhtTiny)
		{
			this.InitializeComponent();
			_dhtTiny = dhtTiny;
		}

		protected async override void OnApplyTemplate()
		{
			base.OnApplyTemplate();

			try
			{
				// ***
				// *** Get and cache the current values.
				// ***
				_interval = await _dhtTiny.GetIntervalAsync();
				_lowerThreshold = await _dhtTiny.GetLowerThresholdAsync();
				_upperThreshold = await _dhtTiny.GetUpperThresholdAsync();
				_startDelay = await _dhtTiny.GetStartDelayAsync();
				_address = await _dhtTiny.GetDeviceAddressAsync();
				_model = await _dhtTiny.GetDeviceModelAsync();

				// ***
				// *** Update the dialog.
				// ***
				this.interval.Value = _interval;
				this.lowerThreshold.Value = _lowerThreshold;
				this.upperThreshold.Value = _upperThreshold;
				this.startDelay.Value = _startDelay;
				this.address.Value = _address;
				this.model.SelectedIndex = _models.Where(t => t.Model == _model).Select(t => t.Index).FirstOrDefault();
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}
		}

		private async void ContentDialog_OkButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
		{
			try
			{
				// ***
				// *** Get the new values from the dialog.
				// ***
				UInt32 interval = Convert.ToUInt32(this.interval.Value);
				float lowerThreshold = Convert.ToUInt32(this.lowerThreshold.Value);
				float upperThreshold = Convert.ToUInt32(this.upperThreshold.Value);
				UInt32 startDelay = Convert.ToUInt32(this.startDelay.Value);
				byte address = Convert.ToByte(this.address.Value);
				byte model = _models.Where(t => t.Index == this.model.SelectedIndex).Select(t => t.Model).FirstOrDefault();

				// ***
				// *** Update only values that have changed.
				// ***
				if (_interval != interval) await _dhtTiny.SetIntervalAsync(interval);
				if (_lowerThreshold != lowerThreshold) await _dhtTiny.SetLowerThresholdAsync(lowerThreshold);
				if (_upperThreshold != upperThreshold) await _dhtTiny.SetUpperThresholdAsync(upperThreshold);
				if (_startDelay != startDelay) await _dhtTiny.SetStartDelayAsync(startDelay);
				if (_address != address) await _dhtTiny.SetDeviceAddressAsync(address);
				if (_model != model) await _dhtTiny.SetDeviceModelAsync(model);
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}
		}
	}
}
