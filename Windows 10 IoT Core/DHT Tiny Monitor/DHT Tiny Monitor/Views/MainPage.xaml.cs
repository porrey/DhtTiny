using System;
using Porrey.Tiny.Dht.Common;
using Porrey.Uwp.IoT.Sensors.Tiny;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;

namespace Porrey.Tiny.Dht
{
	public sealed partial class MainPage : BindablePage
	{
		private DhtTiny _dhtTiny = null;
		private DispatcherTimer _timer = new DispatcherTimer();

		public MainPage()
		{
			this.InitializeComponent();
			InitializeCommands();

			// ***
			// *** Setup the timer.
			// ***
			_timer.Interval = TimeSpan.FromMilliseconds(750);
			_timer.Tick += Timer_Tick;
		}

		protected async override void OnNavigatedTo(NavigationEventArgs e)
		{
			try
			{
				// ***
				// *** Setup the DhtTiny instance.
				// ***
				_dhtTiny = new DhtTiny(0x26);
				await _dhtTiny.InitializeAsync();

				// ***
				// *** Start the timer.
				// ***
				_timer.Start();
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}

			base.OnNavigatedTo(e);
		}

		private void Timer_Tick(object sender, object e)
		{
			try
			{
				lock (_dhtTiny)
				{
					_dhtTiny.Refresh();

					this.Interval = _dhtTiny.Interval;
					this.ReadingId = _dhtTiny.ReadingId;
					this.Temperature = string.Format("{0:0.0}°C", _dhtTiny.Temperature);
					this.Humidity = string.Format("{0:0.0}%", _dhtTiny.Humidity);
					this.UpperThreshold = _dhtTiny.UpperThreshold;
					this.LowerThreshold = _dhtTiny.LowerThreshold;
					this.StartDelay = _dhtTiny.StartDelay;

					this.StatusEnabled = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.IsEnabled) ? "1" : "0";
					this.StatusUpperThresholdExceeded = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.UpperThresholdExceeded) ? "1" : "0";
					this.StatusLowerThresholdExceeded = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.LowerThresholdExceeded) ? "1" : "0";
					this.StatusReserved1 = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.Reserved1) ? "1" : "0";
					this.StatusReserved2 = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.Reserved2) ? "1" : "0";
					this.StatusConfigurationSaved = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.ConfigSaved) ? "1" : "0";
					this.StatusReadError = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.ReadError) ? "1" : "0";
					this.StatusWriteError = _dhtTiny.GetStatusBit(DhtTiny.StatusBit.WriteError) ? "1" : "0";

					this.configEnabled.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.SensorEnabled) ? true : false;
					this.configThresholdEnabled.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.ThresholdEnabled) ? true : false;
					this.configTriggerReading.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.TriggerReading) ? true : false;
					this.reserved1.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.Reserved1) ? true : false;
					this.reserved2.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.Reserved2) ? true : false;
					this.reserved3.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.Reserved3) ? true : false;
					this.writeConfiguration.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.WriteConfig) ? true : false;
					this.resetConfiguration.IsChecked = _dhtTiny.GetConfigurationBit(DhtTiny.ConfigBit.ResetConfig) ? true : false;

					this.EnabledCommand.RaiseCanExecuteChanged();
					this.EnableThresholdsCommand.RaiseCanExecuteChanged();
					this.TriggerReadingCommand.RaiseCanExecuteChanged();
					this.Reserved1Command.RaiseCanExecuteChanged();
					this.Reserved2Command.RaiseCanExecuteChanged();
					this.Reserved3Command.RaiseCanExecuteChanged();
					this.WriteConfigurationCommand.RaiseCanExecuteChanged();
					this.ResetConfigurationCommand.RaiseCanExecuteChanged();
					this.SettingsCommand.RaiseCanExecuteChanged();
				}
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}
		}

		protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
		{
			try
			{
				if (_dhtTiny != null)
				{
					_dhtTiny.Dispose();
					_dhtTiny = null;
				}

				_timer.Stop();
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}

			base.OnNavigatingFrom(e);
		}

		#region Values
		private uint _readingId = 0;
		public uint ReadingId
		{
			get
			{
				return _readingId;
			}
			set
			{
				this.SetProperty(ref _readingId, value);
			}
		}

		private double _interval = 0;
		public double Interval
		{
			get
			{
				return _interval;
			}
			set
			{
				this.SetProperty(ref _interval, value);
			}
		}

		private string _temperature = "0.0°C";
		public string Temperature
		{
			get
			{
				return _temperature;
			}
			set
			{
				this.SetProperty(ref _temperature, value);
			}
		}

		private string _humidity = "0.0%";
		public string Humidity
		{
			get
			{
				return _humidity;
			}
			set
			{
				this.SetProperty(ref _humidity, value);
			}
		}

		private double _upperThreshold = 0;
		public double UpperThreshold
		{
			get
			{
				return _upperThreshold;
			}
			set
			{
				this.SetProperty(ref _upperThreshold, value);
			}
		}

		private double _lowerThreshold = 0;
		public double LowerThreshold
		{
			get
			{
				return _lowerThreshold;
			}
			set
			{
				this.SetProperty(ref _lowerThreshold, value);
			}
		}

		private double _startDelay = 0;
		public double StartDelay
		{
			get
			{
				return _startDelay;
			}
			set
			{
				this.SetProperty(ref _startDelay, value);
			}
		}

		private string _statusEnabled = "0";
		public string StatusEnabled
		{
			get
			{
				return _statusEnabled;
			}
			set
			{
				this.SetProperty(ref _statusEnabled, value);
			}
		}

		private string _statusUpperThresholdExceeded = "0";
		public string StatusUpperThresholdExceeded
		{
			get
			{
				return _statusUpperThresholdExceeded;
			}
			set
			{
				this.SetProperty(ref _statusUpperThresholdExceeded, value);
			}
		}

		private string _statusLowerThresholdExceeded = "0";
		public string StatusLowerThresholdExceeded
		{
			get
			{
				return _statusLowerThresholdExceeded;
			}
			set
			{
				this.SetProperty(ref _statusLowerThresholdExceeded, value);
			}
		}

		private string _statusReserved1 = "0";
		public string StatusReserved1
		{
			get
			{
				return _statusReserved1;
			}
			set
			{
				this.SetProperty(ref _statusReserved1, value);
			}
		}

		private string _statusReserved2 = "0";
		public string StatusReserved2
		{
			get
			{
				return _statusReserved2;
			}
			set
			{
				this.SetProperty(ref _statusReserved2, value);
			}
		}

		private string _statusConfigurationSaved = "0";
		public string StatusConfigurationSaved
		{
			get
			{
				return _statusConfigurationSaved;
			}
			set
			{
				this.SetProperty(ref _statusConfigurationSaved, value);
			}
		}

		private string _statusReadError = "0";
		public string StatusReadError
		{
			get
			{
				return _statusReadError;
			}
			set
			{
				this.SetProperty(ref _statusReadError, value);
			}
		}

		private string _statusWriteError = "0";
		public string StatusWriteError
		{
			get
			{
				return _statusWriteError;
			}
			set
			{
				this.SetProperty(ref _statusWriteError, value);
			}
		}
		#endregion

		#region Commands
		public RelayCommand EnabledCommand { get; set; }
		public RelayCommand EnableThresholdsCommand { get; set; }
		public RelayCommand TriggerReadingCommand { get; set; }
		public RelayCommand Reserved1Command { get; set; }
		public RelayCommand Reserved2Command { get; set; }
		public RelayCommand Reserved3Command { get; set; }
		public RelayCommand WriteConfigurationCommand { get; set; }
		public RelayCommand ResetConfigurationCommand { get; set; }
		public RelayCommand SettingsCommand { get; set; }

		private void InitializeCommands()
		{
			try
			{
				this.EnabledCommand = new RelayCommand(OnEnabledCommand, OnCanEnabledCommand);
				this.EnableThresholdsCommand = new RelayCommand(OnEnableThresholdsCommand, OnCanEnableThresholdsCommand);
				this.TriggerReadingCommand = new RelayCommand(OnTriggerReadingCommand, OnCanTriggerReadingCommand);
				this.Reserved1Command = new RelayCommand(OnReserved1Command, OnCanReserved1Command);
				this.Reserved2Command = new RelayCommand(OnReserved2Command, OnCanReserved2Command);
				this.Reserved3Command = new RelayCommand(OnReserved3Command, OnCanReserved3Command);
				this.WriteConfigurationCommand = new RelayCommand(OnWriteConfigurationCommand, OnCanWriteConfigurationCommand);
				this.ResetConfigurationCommand = new RelayCommand(OnResetConfigurationCommand, OnCanResetConfigurationCommand);
				this.SettingsCommand = new RelayCommand(OnSettingsCommand, OnCanSettingsCommand);
			}
			catch (Exception ex)
			{
				(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
			}
		}

		private bool OnCanSettingsCommand()
		{
			return _dhtTiny.IsInitialized && _dhtTiny.IsEnabled;
		}

		private void OnSettingsCommand()
		{
		}

		private bool OnCanEnabledCommand()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnEnabledCommand()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.SensorEnabled, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.SensorEnabled));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanEnableThresholdsCommand()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnEnableThresholdsCommand()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.ThresholdEnabled, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.ThresholdEnabled));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanTriggerReadingCommand()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnTriggerReadingCommand()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.TriggerReading, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.TriggerReading));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanReserved1Command()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnReserved1Command()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.Reserved1, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.Reserved1));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanReserved2Command()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnReserved2Command()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.Reserved2, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.Reserved2));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanReserved3Command()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnReserved3Command()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.Reserved3, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.Reserved3));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanWriteConfigurationCommand()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnWriteConfigurationCommand()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.WriteConfig, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.WriteConfig));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}

		private bool OnCanResetConfigurationCommand()
		{
			return _dhtTiny.IsInitialized;
		}

		private void OnResetConfigurationCommand()
		{
			lock (_dhtTiny)
			{
				try
				{
					_timer.Stop();
					_dhtTiny.SetConfiguration((byte)DhtTiny.ConfigBit.ResetConfig, !_dhtTiny.GetConfiguration((byte)DhtTiny.ConfigBit.ResetConfig));
				}
				catch (Exception ex)
				{
					(new MessageDialog(ex.Message)).ShowAsync().AsTask().Wait();
				}
				finally
				{
					_timer.Start();
				}
			}
		}
		#endregion
	}
}
