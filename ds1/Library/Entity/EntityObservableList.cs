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
	using DemonSaw.Collection;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Diagnostics;
	using System.Linq;
	using System.Windows.Threading;

	public sealed class EntityObservableList : EntityIdle, IEnumerable, IEnumerable<Entity>
	{
		#region Properties
		public ObservableList<Entity> List { get; set; }
		public ObservableCollection<Entity> Collection { get { return List.Collection; } }
		public bool Empty { get { return List.Count <= 0; } }
		public int Count { get { return List.Count; } }

		public bool Valid
		{
			get
			{
				lock (List)
				{
					foreach (Entity entity in List)
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
		public EntityObservableList() { List = new ObservableList<Entity>(); }
		public EntityObservableList(Dispatcher dispatcher) { List = new ObservableList<Entity>(dispatcher); }
		#endregion

		#region System
		public IEnumerator<Entity> GetEnumerator()
		{
			return List.GetEnumerator();
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return List.GetEnumerator();
		}
		#endregion

		#region Interface
		public void Update()
		{
			lock (List)
			{
				foreach (Entity entity in List)
					entity.Update();
			}
		}
		#endregion

		#region Container
		public void Add(Entity entity)
		{
			Debug.Assert(entity != null);

			EntityIdleComponent idle = entity.Get<EntityIdleComponent>();
			if (idle != null)
				idle.Add(this);

			lock (List)
				List.Add(entity);

			entity++;
		}

		public bool Remove(Entity entity)
		{
			Debug.Assert(entity != null);

			bool removed;
			lock (List)
				removed = List.Remove(entity);

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
			lock (List)
			{
				List<Entity> list = new List<Entity>(List);
				foreach (Entity entity in list)
					Remove(entity);

				Debug.Assert(Empty);
			}
		}

		public bool Contains(Entity entity)
		{
			Debug.Assert(entity != null);

			lock (List)
				return List.Contains(entity);
		}

		public Entity Get(int index)
		{
			lock (List)
				return !Empty ? List[index] : null;
		}
		#endregion

		#region Utility
		public void Sort()
		{
			lock (List)
				List.Sort();
		}

		public void Sort(Comparison<Entity> comparison)
		{
			lock (List)
				List.Sort(comparison);
		}

		public Entity Find(Func<Entity, bool> condition)
		{
			lock (List)
				return List.Find(condition);
		}

		public IEnumerable<Entity> FindAll(Func<Entity, bool> condition)
		{
			lock (List)
				return List.Where(condition).ToList();
		}
		#endregion
	}
}
