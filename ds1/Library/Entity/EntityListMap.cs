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

	public sealed class EntityListMap : EntityIdle
	{
		#region Properties
		public Dictionary<string, EntityList> Map { get; set; }
		public Dictionary<string, EntityList>.KeyCollection Keys { get { return Map.Keys; } }
		public Dictionary<string, EntityList>.ValueCollection Values { get { return Map.Values; } }		
		public bool Empty { get { return Map.Count <= 0; } }
		public int Count { get { return Map.Count; } }

		public bool Valid
		{
			get
			{
				lock (Map)
				{
					foreach (EntityList list in Map.Values)
					{
						if (!list.Valid)
							return false;
					}

					return true;
				}
			}
		}
		#endregion

		#region Constructors
		public EntityListMap() { Map = new Dictionary<string, EntityList>(); }
		#endregion

		#region Interface
		public void Update()
		{
			lock (Map)
			{
				foreach (EntityList list in Map.Values)
					list.Update();
			}
		}
		#endregion

		#region Container
		public void Add(string id)
		{
			Debug.Assert(id != null);

			lock (Map)
			{
				EntityList list = Get(id);

				if (list == null)
				{
					list = new EntityList();
					Map.Add(id, list);
				}
			}
		}

		public void Add(string id, Entity entity)
		{
			Debug.Assert(id != null && entity != null);

			lock (Map)
			{
				EntityList list = Get(id);

				if (list == null)
				{
					list = new EntityList();
					Map.Add(id, list);
				}

				EntityIdleComponent idle = entity.Get<EntityIdleComponent>();
				if (idle != null)
					idle.Add(this);

				list.Add(entity);
				list.Sort();

				entity++;
			}
		}

		public bool Remove(string id)
		{
			Debug.Assert(id != null);

			bool removed = false;

			lock (Map)
			{
				EntityList list = Get(id);

				if (list != null)
				{
					list.Clear();
					removed = Map.Remove(id);
				}
			}

			return removed;
		}

		public bool Remove(Entity entity)
		{
			Debug.Assert(entity != null);

			bool removed = false;

			lock (Map)
			{
				string key = null;
				List<string> keys = new List<string>(Map.Keys);

				foreach (string id in keys)
				{
					EntityList list = Get(id);

					if (list != null)
					{
						removed = list.Remove(entity);
						if (list.Count <= 0)
							key = id;

						break;
					}
					
				}

				if (key != null)
					Map.Remove(key);
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

		public bool Remove(string id, Entity entity)
		{
			Debug.Assert(id != null && entity != null);

			bool removed = false;

			lock (Map)
			{
				EntityList list = Get(id);

				if (list != null)
				{
					removed = list.Remove(entity);
					if (list.Count <= 0)
						Map.Remove(id);
				}
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

		public void Clear()
		{
			lock (Map)
			{
				List<string> keys = new List<string>(Map.Keys);

				foreach (string id in keys)
					Remove(id);

				Debug.Assert(Empty);
			}
		}

		public bool Contains(string id)
		{
			Debug.Assert(id != null);

			lock (Map) 
				return Map.ContainsKey(id);
		}

		public bool Contains(string id, Entity entity)
		{
			Debug.Assert(id != null && entity != null);

			EntityList list;
			lock (Map) 
				Map.TryGetValue(id, out list);

			return (list != null) ? list.Contains(entity) : false;
		}

		public EntityList Get(string id)
		{
			Debug.Assert(id != null);

			EntityList list;
			lock (Map)
				Map.TryGetValue(id, out list);

			return list;
		}

		public EntityList Get(Entity entity)
		{
			Debug.Assert(entity != null);

			lock (Map)
			{
				foreach (EntityList list in Map.Values)
				{
					if (list.Contains(entity))
						return list;
				}
			}

			return null;
		}
		#endregion
	}
}
