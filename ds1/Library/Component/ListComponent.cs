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
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.Linq;

	public class ListComponent<T> : EntityComponent, IEnumerable, IEnumerable<T>
	{
		// Variables
		protected List<T> _list = new List<T>();

		// Properties - Helper
		public int Count { get { return _list.Count; } }
		public bool Empty { get { return _list.Count <= 0; } }
		public List<T> List { get { return _list; } }

		#region System
		public IEnumerator<T> GetEnumerator()
		{
			return _list.GetEnumerator();
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return _list.GetEnumerator();
		}
		#endregion

		#region Container
		public virtual void Add(T item)
		{
			Debug.Assert(item != null);

			lock (_list)
				_list.Add(item);
		}

		public virtual bool Remove(T item)
		{
			Debug.Assert(item != null);

			lock (_list)
				return _list.Remove(item);
		}

		public virtual void Clear()
		{
			lock (_list)
				_list.Clear();
		}

		public bool Contains(T item)
		{
			Debug.Assert(item != null);

			lock (_list)
				return _list.Contains(item);
		}

		public T Get(int index)
		{
			lock (_list)
				return _list[index];
		}
		#endregion

		#region Utility
		public void Sort()
		{
			lock (_list)
				_list.Sort();
		}

		public void Sort(Comparison<T> comparison)
		{
			lock (_list)
				_list.Sort(comparison);
		}

		public T Find(Func<T, bool> condition)
		{
			lock (_list)
			{
				List<T> items = _list.Where(condition).ToList();
				return (items.Count > 0) ? items[0] : default(T);
			}
		}

		public IEnumerable<T> FindAll(Func<T, bool> condition)
		{
			lock (_list)
				return _list.Where(condition).ToList();
		}
		#endregion
	}
}
