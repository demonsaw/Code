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
	using DemonSaw.Entity;
	using DemonSaw.Utility;
	using System.Collections.Generic;
	using System.Xml;	

	public abstract class NetworkData : MachineData
	{
		// Variables
		private string _address;
		private int _port;

		// Properties		
		public Dictionary<string, Entity> Entities { get; set; }

		// Properties - Helper
		public override bool Valid { get { return !string.IsNullOrEmpty(Address) && (Port > 0); } }

		public virtual string Address
		{
			get { return _address; }
			set
			{
				_address = value;
				OnPropertyChanged("Address");
			}
		}

		public virtual int Port
		{
			get { return _port; }
			set
			{
				_port = value;
				OnPropertyChanged("Port");
			}
		}

		// Constructors
		protected NetworkData() { Init(); }
		protected NetworkData(string id) : base(id) { Init(); }
		protected NetworkData(string id, string name) : base(id, name) { Init(); }
		protected NetworkData(NetworkData data) : base(data) { Init(); }

		// Interface
		private void Init()
		{
			Address = Default.Address;
			Port = Default.Port;

			Entities = new Dictionary<string, Entity>();
		}

		public override void Clear()
		{
			base.Clear();

			Name = Default.Name;

			Address = Default.Address;
			Port = Default.Port;
			
			Entities.Clear();
		}

		public abstract void Read(XmlReader reader);
		public abstract void Write(XmlWriter writer);

		// Entities
		public bool HasEntity(string key)
		{
			lock (Entities)
				return Entities.ContainsKey(key);
		}

		public void AddEntity(string key, Entity entity)
		{
			lock (Entities)
				Entities.Add(key, entity);
		}

		public bool RemoveEntity(string key)
		{
			lock (Entities)
				return Entities.Remove(key);
		}

		public Entity GetEntity(string key)
		{
			Entity entity = null;

			lock (Entities)
				Entities.TryGetValue(key, out entity);

			return entity;
		}
	}
}
