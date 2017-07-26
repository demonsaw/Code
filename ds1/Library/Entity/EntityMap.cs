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
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.Linq;

	public sealed class EntityMap : EntityIdle
	{
		#region Properties
		public Dictionary<string, Entity> Map { get; set; }
		public Dictionary<string, Entity>.KeyCollection Keys { get { return Map.Keys; } }
		public Dictionary<string, Entity>.ValueCollection Values { get { return Map.Values; } }		
		public bool Empty { get { return Map.Count <= 0; } }
		public int Count { get { return Map.Count; } }

		public bool Valid
		{
			get
			{
				lock (Map)
				{
					foreach (Entity entity in Map.Values)
					{
						if (!entity.Valid)
							return false;
					}

					return true;
				}
			}
		}
		#endregion

		#region Constructors
		public EntityMap() { Map = new Dictionary<string, Entity>(); }
		#endregion

		#region Interface
		public void Update()
		{
			lock (Map)
			{
				foreach (Entity entity in Map.Values)
					entity.Update();
			}
		}
		#endregion

		#region Container
		public void Add(Entity entity)
		{
			Debug.Assert(entity != null);

			Add(entity.Id, entity);
		}

		public void Add(string id, Entity entity)
		{
			Debug.Assert(id != null && entity != null);

			EntityIdleComponent idle = entity.Get<EntityIdleComponent>();
			if (idle != null)
				idle.Add(this);

			Remove(id);

			lock (Map)
				Map.Add(id, entity);

			entity++;
		}

		public bool Remove(Entity entity)
		{
			Debug.Assert(entity != null);

			bool removed = false;

			lock (Map)
			{
				string id = Get(entity);
				if (id != null)
					removed = Map.Remove(id);
			}				

			if (removed)
			{
				EntityIdleComponent idle = entity.Get<EntityIdleComponent>();
				if (idle != null)
					idle.Remove(this);

				entity--;
			}				

			return removed;
		}

		public bool Remove(string id)
		{
			Debug.Assert(id != null);

			bool removed = false;
			Entity entity = Get(id);

			if (entity != null)
			{
				lock (Map)
					removed = Map.Remove(id);

				if (removed)
				{
					EntityIdleComponent idle = entity.Get<EntityIdleComponent>();
					if (idle != null)
						idle.Remove(this);

					entity--;
				}
			}

			return removed;
		}

		public void Clear()
		{
			lock (Map)
			{
				List<string> list = new List<string>(Map.Keys);
				foreach (string id in list)
					Map.Remove(id);

				Debug.Assert(Empty);
			}
		}

		public bool Contains(string id)
		{
			Debug.Assert(id != null);

			lock (Map)
				return Map.ContainsKey(id);
		}

		public bool Contains(Entity entity)
		{
			Debug.Assert(entity != null);

			lock (Map)
				return Map.ContainsValue(entity);
		}

		public Entity Get(string id)
		{
			Debug.Assert(id != null);

			Entity entity;
			lock (Map)
				Map.TryGetValue(id, out entity);

			return entity;
		}

		public string Get(Entity entity)
		{
			Debug.Assert(entity != null);

			lock (Map)
				return Map.FirstOrDefault(i => i.Value == entity).Key;
		}
		#endregion
	}
}
