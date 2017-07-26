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

namespace DemonSaw.Component
{
	using DemonSaw.Entity;
	using DemonSaw.Json.Object;
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using System;

	public sealed class ClientComponent : EntityComponent, IComparable<ClientComponent>
	{
		#region Variables
		private string _id;
		private string _name;
		private string _download;
		private bool _enabled;
		private bool _selected;
		#endregion

		#region Properties
		public string Id
		{
			get { return _id; }
			set
			{
				_id = value;
				OnPropertyChanged("Id");
			}
		}

		public string Name
		{
			get { return _name; }
			set
			{
				_name = value;
				OnPropertyChanged("Name");				
			}
		}

		public string Download
		{
			get { return _download; }
			set
			{
				_download = value;
				OnPropertyChanged("Download");
			}
		}

		public bool Enabled
		{
			get { return _enabled; }
			set
			{
				_enabled = value;
				OnPropertyChanged("Enabled");
			}
		}

		public bool Selected
		{
			get { return _selected; }
			set
			{
				_selected = value;
				OnPropertyChanged("Selected");
			}
		}

		public bool Disabled
		{
			get { return !Enabled; }
		}

		public override bool Valid
		{
			get { return base.Valid && Enabled && !string.IsNullOrEmpty(Id) && !string.IsNullOrEmpty(Name) && !string.IsNullOrEmpty(Download); }
		}
		#endregion

		#region Constructors
		public ClientComponent() : this(SecurityUtil.CreateKeyString()) { }

		public ClientComponent(string id) : this(id, Default.Name) { }

		public ClientComponent(string id, string name)
		{
			Id = id;
			Name = name;
			Download = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Downloads");
			Enabled = Default.Enabled;
		}
		#endregion

		#region Operators
		public static implicit operator JsonClient(ClientComponent client) { return new JsonClient(client.Id, client.Name); }
		#endregion

		#region System
		public override string ToString()
		{
			return Name;
		}

		public int CompareTo(ClientComponent client)
		{
			return Name.CompareTo(client.Name);
		}
		#endregion
	}
}
