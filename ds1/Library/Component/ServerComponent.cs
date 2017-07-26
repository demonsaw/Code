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

	public sealed class ServerComponent : EntityComponent
	{
		#region Variables
		private string _id;
		private string _name;
		private bool _enabled;

		private int _entities;
		private int _sessions;
		private int _clients;
		private int _groups;
		
		private string _passphrase;
		private string _address;
		private int _port;
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

		public bool Enabled
		{
			get { return _enabled; }
			set
			{
				_enabled = value;
				OnPropertyChanged("Enabled");
			}
		}

		public int Entities
		{
			get { return _entities; }
			set
			{
				_entities = value;
				OnPropertyChanged("Entities");
			}
		}

		public int Sessions
		{
			get { return _sessions; }
			set
			{
				_sessions = value;
				OnPropertyChanged("Sessions");
			}
		}

		public int Clients
		{
			get { return _clients; }
			set
			{
				_clients = value;
				OnPropertyChanged("Clients");
			}
		}

		public int Groups
		{
			get { return _groups; }
			set
			{
				_groups = value;
				OnPropertyChanged("Groups");
			}
		}

		public string Passphrase
		{
			get { return _passphrase; }
			set
			{
				_passphrase = value;
				OnPropertyChanged("Passphrase");
			}
		}

		public string Address
		{
			get { return _address; }
			set
			{
				_address = value;
				OnPropertyChanged("Address");
			}
		}

		public int Port
		{
			get { return _port; }
			set
			{
				_port = value;
				OnPropertyChanged("Port");
			}
		}

		public bool Disabled 
		{ 
			get { return !Enabled; } 
		}
		
		public override bool Valid 
		{ 
			get { return base.Valid && Enabled && !string.IsNullOrEmpty(Id) && !string.IsNullOrEmpty(Address) && (Port > 0); } 
		}
		#endregion

		#region Constructors
		public ServerComponent() 
		{ 
			_id = SecurityUtil.CreateKeyString();
			_name = Default.Name;
			_enabled = Default.Enabled;

			_passphrase = Default.Passphrase;
			_address = Default.Address;
			_port = Default.Port;
		}
		
		public ServerComponent(string id)
		{ 
			_id = id;
			_name = Default.Name;
			_enabled = Default.Enabled;

			_passphrase = Default.Passphrase; 
			_address = Default.Address;
			_port = Default.Port;
		}

		public ServerComponent(string id, string name)
		{ 
			_id = id;
			_name = name;
			_enabled = Default.Enabled;

			_passphrase = Default.Passphrase; 
			_address = Default.Address;
			_port = Default.Port;
		}
		#endregion

		#region Operators
		public static implicit operator JsonServer(ServerComponent server) { return new JsonServer(server.Id, server.Name); }
		#endregion
	}
}
