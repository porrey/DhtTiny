// Copyright © 2016 Daniel Porrey
//
// This file is part of the Star Wars 3D Art Solution.
// 
// Star Wars 3D Art Solution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Star Wars 3D Art Solution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Star Wars 3D Art Solution. If not, see http://www.gnu.org/licenses/.
//
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Windows.ApplicationModel.Resources;
using Windows.UI.Xaml.Controls;

namespace Porrey.Tiny.Dht.Common
{
	public abstract class BindablePage : Page, INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged = null;

		protected void OnPropertyChanged([CallerMemberName]string propertyName = null)
		{
			if (this.PropertyChanged != null)
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}

		protected bool SetProperty<T>(ref T storage, T value, [CallerMemberName] string propertyName = null)
		{
			if (object.Equals(storage, value)) return false;

			storage = value;
			this.OnPropertyChanged(propertyName);
			return true;
		}

		protected ResourceLoader ResourceLoader => ResourceLoader.GetForCurrentView();
	}
}
