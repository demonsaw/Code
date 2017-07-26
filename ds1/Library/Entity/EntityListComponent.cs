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
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Diagnostics;

	public class EntityListComponent : EntityComponent, IEnumerable, IEnumerable<Entity>
	{
		#region Variables
		private EntityList _list;
		#endregion

		#region Properties
		public List<Entity> List { get { return _list.List; } }
		public bool Empty { get { return _list.Empty; } }
		public int Count { get { return _list.Count; } }

		public override bool Valid { get { return base.Valid && _list.Valid; } }
		#endregion

		#region Constructors
		public EntityListComponent() { _list = new EntityList(); }
		#endregion

		#region System
		public IEnumerator<Entity> GetEnumerator()
		{
			return _list.GetEnumerator();
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return _list.GetEnumerator();
		}
		#endregion

		#region Interface
		public override void Update()
		{
			base.Update();

			_list.Update();
		}

		public override void Shutdown()
		{
			base.Shutdown();

			Clear();
		}
		#endregion

		#region Container
		public void Add(Entity entity)
		{
			Debug.Assert(entity != Owner);

			_list.Add(entity);
		}

		public bool Remove(Entity entity)
		{
			Debug.Assert(entity != Owner);

			return _list.Remove(entity);
		}

		public void Clear()
		{
			_list.Clear();
		}

		public bool Contains(Entity entity)
		{
			Debug.Assert(entity != Owner);

			return _list.Contains(entity);
		}

		public Entity Get(int index)
		{
			return _list.Get(index);
		}
		#endregion

		#region Utility
		public void Sort()
		{
			_list.Sort();
		}

		public void Sort(Comparison<Entity> comparison)
		{
			_list.Sort(comparison);
		}

		public Entity Find(Func<Entity, bool> condition)
		{
			return _list.Find(condition);
		}

		public IEnumerable<Entity> FindAll(Func<Entity, bool> condition)
		{
			return _list.FindAll(condition);
		}
		#endregion
	}
}
