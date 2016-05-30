using Porrey.Tiny.Dht.Common;
using Windows.UI.Xaml.Controls;


namespace Porrey.Tiny.Dht
{
	public sealed partial class MainPage : BindablePage
	{
		public MainPage()
        {
            this.InitializeComponent();
			InitializeCommands();
		}

		#region Values
		private int _readingId = 0;
		public int ReadingId
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

		public RelayCommand RebootCommand { get; set; }

		private void InitializeCommands()
		{
			this.EnabledCommand = new RelayCommand(OnReboot, OnCanReboot);
			this.EnableThresholdsCommand = new RelayCommand(OnReboot, OnCanReboot);
			this.TriggerReadingCommand = new RelayCommand(OnReboot, OnCanReboot);
			this.Reserved1Command = new RelayCommand(OnReboot, OnCanReboot);
			this.Reserved2Command = new RelayCommand(OnReboot, OnCanReboot);
			this.Reserved3Command = new RelayCommand(OnReboot, OnCanReboot);
			this.WriteConfigurationCommand = new RelayCommand(OnReboot, OnCanReboot);
			this.ResetConfigurationCommand = new RelayCommand(OnReboot, OnCanReboot);

			this.RebootCommand = new RelayCommand(OnReboot, OnCanReboot);
		}

		private bool OnCanReboot()
		{
			return false;
		}

		private void OnReboot()
		{
		}
		#endregion
	}
}
