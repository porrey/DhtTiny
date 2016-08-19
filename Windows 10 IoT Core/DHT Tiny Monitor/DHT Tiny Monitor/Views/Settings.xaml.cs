using System;
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

				// ***
				// *** Update the dialog.
				// ***
				this.interval.Value = _interval;
				this.lowerThreshold.Value = _lowerThreshold;
				this.upperThreshold.Value = _upperThreshold;
				this.startDelay.Value = _startDelay;
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

				// ***
				// *** Update only values that have changed.
				// ***
				if (_interval != interval) await _dhtTiny.SetIntervalAsync(interval);
				if (_lowerThreshold != lowerThreshold) await _dhtTiny.SetLowerThresholdAsync(lowerThreshold);
				if (_upperThreshold != upperThreshold) await _dhtTiny.SetUpperThresholdAsync(upperThreshold);
				if (_startDelay != startDelay) await _dhtTiny.SetStartDelayAsync(startDelay);
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}
		}
	}
}
