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

namespace DemonSaw.Entity
{
	using DemonSaw.Component;
	using DemonSaw.Json.Object;
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using System;
	using System.Collections.Generic;
	using System.Diagnostics;

	// Delegates	
	public delegate void EntityEventHandler();	

	public sealed class Entity : IState, IComparable<Entity>, IDisposable
	{
		#region Variables
		private Dictionary<Type, EntityComponent> _map = new Dictionary<Type, EntityComponent>();
		#endregion

		#region Events
		public event EntityEventHandler EntityInit;
		public event EntityEventHandler EntityUpdate; 
		public event EntityEventHandler EntityShutdown;
		public event EntityEventHandler EntityChanged;
		#endregion

		#region Properties
		public string Id { get; set; }
		public string Name { get; set; }
		public int Ref { get; private set; }	

		public int Count { get { return _map.Count; } }
		public bool Empty { get { return _map.Count <= 0; } }

		public bool Active { get; private set; }
		public bool Inactive { get { return !Active; } }

		public bool Valid
		{
			get
			{
				lock (_map)
				{
					foreach (EntityComponent component in _map.Values)
					{
						if (!component.Valid)
							return false;
					}

					return true;
				}
			}
		}

		public bool Invalid { get { return !Valid; } }
		#endregion

		#region Constructors
		public Entity() { Id = SecurityUtil.CreateKeyString(); }		
		public Entity(string id) { Id = id; }
		public Entity(string id, string name) { Id = id; Name = name; }
		~Entity() { Shutdown(); }
		#endregion

		#region System
		void IDisposable.Dispose()
		{
			Shutdown();
		}

		public override string ToString()
		{
			return Name;
		}

		public int CompareTo(Entity entity)
		{
			return Id.CompareTo(entity.Id);
		}

		public bool Equals(Entity entity)
		{
			if (ReferenceEquals(null, entity))
				return false;

			if (ReferenceEquals(this, entity))
				return true;

			return Equals(entity.Id, Id);
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
				return false;

			if (ReferenceEquals(this, obj))
				return true;

			if (!(obj is Entity))
				return false;

			return Equals((Entity) obj);
		}

		public override int GetHashCode()
		{
			return (Id != null) ? Id.GetHashCode() : base.GetHashCode();
		}
		#endregion

		#region Operators
		public static implicit operator JsonEntity(Entity entity) { return new JsonEntity(entity.Id, entity.Name); }

		public static Entity operator ++(Entity entity)
		{
			entity.Ref++;

			return entity;
		}

		public static Entity operator --(Entity entity)
		{
			entity.Ref--;
			Debug.Assert(entity.Ref >= 0);

			if (entity.Ref == 0)
				entity.Shutdown();

			return entity;
		}
		#endregion

		#region Events
		private void OnEntityInit()
		{
			if (EntityInit != null)
				EntityInit();
		}

		private void OnEntityUpdate()
		{
			if (EntityUpdate != null)
				EntityUpdate();
		}

		private void OnEntityShutdown()
		{
			if (EntityShutdown != null)
				EntityShutdown();
		}

		private void OnEntityChanged()
		{
			if (EntityChanged != null)
				EntityChanged();
		}
		#endregion

		#region Interface
		public void Init()
		{
			Active = true;

			lock (_map)
			{
				foreach (EntityComponent component in _map.Values)
				{
					component.Owner = this;
					component.Init();
				}
			}

			OnEntityInit();
		}

		public void Update()
		{
			lock (_map)
			{
				foreach (EntityComponent component in _map.Values)
					component.Update();
			}

			OnEntityUpdate();
		}

		public void Shutdown()
		{
			lock (_map)
			{
				foreach (EntityComponent component in _map.Values)
					component.Shutdown();
			}
			
			OnEntityShutdown();

			Active = false;
		}

		public void Clear()
		{
			Shutdown();

			lock (_map)
				_map.Clear();
		}
		#endregion

		#region Container
		public void Add<T>() where T : EntityComponent, new()
		{
			Add<T>(new T());
		}

		public void Add<T>(T component) where T : EntityComponent
		{
			Debug.Assert(component != null);
			Debug.Assert(component.Owner == null);
			
			component.Owner = this; 
			component.Init();

			Type type = typeof(T);

			lock (_map)
			{
				if (!_map.ContainsKey(type))
				{
					Active = true;
					_map.Add(type, component);
				}
				else
				{
					_map[type].Shutdown();
					_map[type] = component;
				}
			}
			
			OnEntityChanged();
		}

		public bool Remove<T>() where T : EntityComponent 
		{
			Type type = typeof(T);
			return Remove(type);
		}

		public bool Remove(Type type)
		{
			EntityComponent component = null;
			bool removed = false;

			lock (_map)
			{
				if (_map.TryGetValue(type, out component))
				{
					removed = _map.Remove(type);
					if (removed)
						Active = !Empty;

					component.Shutdown();
					OnEntityChanged();
				}
			}

			return removed;
		}

		public T Get<T>() where T : EntityComponent
		{			
			Type type = typeof(T);
			EntityComponent component = null;
			lock (_map)
				_map.TryGetValue(type, out component);
			return (T) component;
		}

		public bool Contains<T>() where T : EntityComponent 
		{
			Type type = typeof(T);
			lock (_map)
				return _map.ContainsKey(type);
		}
		#endregion

		#region WPF
		public ClientComponent Client 
		{ 
			get { return Get<ClientComponent>(); } 
		}
		
		public ServerComponent Server 
		{ 
			get { return Get<ServerComponent>(); } 
		}

		public MachineComponent Machine 
		{ 
			get { return Get<MachineComponent>(); } 
		}

		public SessionComponent Session 
		{ 
			get { return Get<SessionComponent>(); } 
		}

		public GroupComponent Group 
		{ 
			get { return Get<GroupComponent>(); } 
		}

		public FileComponent File
		{
			get { return Get<FileComponent>(); }
		}

		public TransferComponent Transfer
		{
			get { return Get<TransferComponent>(); }
		}
		#endregion
	}
}
