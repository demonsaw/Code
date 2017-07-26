//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

namespace DemonSaw.Data
{
	using DemonSaw.Json.Object;
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using System;
	using System.ComponentModel;

	public class ItemData : DemonData, IComparable<ItemData>, INotifyPropertyChanged
	{
		// Variables
		protected string _name;
		protected bool _enabled;
		protected DemonStatus _status;		

		// Properties
		public string Id { get; set; }

		// Properties - Helper
		public virtual string Name 
		{
			get { return _name; }
			set
			{ 
				_name = value;
				OnPropertyChanged("Name");
			}
		}

		public DemonStatus Status
		{
			get { return _status; }
			set
			{
				_status = value;
				OnPropertyChanged("Status");
			}
		}

		public virtual bool Enabled
		{
			get { return _enabled; }
			set
			{
				_enabled = value;
				OnPropertyChanged("Enabled");
			}
		}

		public override DateTime Created
		{
			get { return base.Created; }
			set
			{
				base.Created = value;
				Updated = value;
				OnPropertyChanged("Created");
			}
		}

		public override DateTime Updated
		{
			get { return base.Updated; }
			set
			{
				base.Updated = value;
				OnPropertyChanged("Updated");
				OnPropertyChanged("Elapsed");
			}
		}

		public virtual bool Valid { get { return Default.Valid; } }
		public bool Invalid { get { return !Valid; } }

		public bool None { get { return Status == DemonStatus.None; } }		
		public bool Info { get { return Status == DemonStatus.Info; } }
		public bool Paused { get { return Status == DemonStatus.Paused; } }
		public bool Success { get { return Status == DemonStatus.Success; } }
		public bool Warning { get { return Status == DemonStatus.Warning; } }
		public bool Error { get { return Status == DemonStatus.Error; } }
		public bool Cancelled { get { return Status == DemonStatus.Cancelled; } }

		// Event
		public event PropertyChangedEventHandler PropertyChanged;

		// Constructors
		public ItemData()
		{
			Id = SecurityUtil.CreateKeyString();
			Name = Default.Name;
			Init();
		}

		public ItemData(string id)
		{
			Id = id;
			Name = Default.Name;
			Init();
		}

		public ItemData(string id, string name)
		{
			Id = id;
			Name = name;
			Init();
		}

		public ItemData(JsonObject json)
		{
			Id = json.Id;
			Name = json.Name;
			Init();
		}

		public ItemData(ItemData data)
		{
			Id = data.Id;
			Name = data.Name;
			Enabled = data.Enabled;
			Created = data.Created;
			Updated = data.Updated;
		}

		// Override
		public override string ToString() { return Name; }

		public int CompareTo(ItemData data)
		{
			return Name.CompareTo(data.Id);
		}

		public bool Equals(ItemData data)
		{
			if (ReferenceEquals(null, data))
				return false;

			if (ReferenceEquals(this, data))
				return true;

			return Equals(data.Id, Id);
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
				return false;

			if (ReferenceEquals(this, obj))
				return true;

			if (!(obj is ItemData))
				return false;

			return Equals((ItemData) obj);
		}

		public override int GetHashCode()
		{
			return (Id != null) ? Id.GetHashCode() : base.GetHashCode();
		}

		// Events
		public void OnPropertyChanged(string name)
		{
			PropertyChangedEventHandler handler = PropertyChanged;

			if (handler != null)
				handler(this, new PropertyChangedEventArgs(name));
		}

		// Interface
		private void Init()
		{
			Enabled = Default.Enabled;
			Created = Updated = DateTime.Now;
		}
		
		public override void Clear()
		{
			base.Clear();

			Id = SecurityUtil.CreateKeyString();
			Name = Default.Name;
			Enabled = Default.Enabled;
			Status = DemonStatus.None;			
		}
	}
}
