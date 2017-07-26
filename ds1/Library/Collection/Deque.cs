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

namespace DemonSaw.Collection
{
	using System.Collections.Generic;

	public sealed class Deque<T>
	{
		// Variables
		private List<T> _list;

		// Properties
		public int Count { get { return _list.Count; } }
		public bool Empty { get { return _list.Count <= 0; } }

		// Constructors
		public Deque() { _list = new List<T>(); }
		public Deque(int capacity) { _list = new List<T>(capacity); }

		// Utility
		public void Clear() { _list.Clear(); }

		// Push
		public void Push(T item) { PushBack(item); }
		public void PushFront(T item) { _list.Insert(0, item); }
		public void PushBack(T item) { _list.Add(item); }

		// Pop
		public T Pop() { return PopFront(); }
		public T PopFront()
		{
			T item = default(T);

			if (_list.Count > 0)
			{
				item = _list[0];
				_list.RemoveAt(0);
			}

			return item;
		}

		public T PopBack()
		{
			T item = default(T);

			if (_list.Count > 0)
			{
				item = _list[_list.Count - 1];
				_list.RemoveAt(_list.Count - 1);
			}

			return item;
		}

		// Peek
		public T Peek() { return PeekFront(); }
		public T PeekFront() { return (_list.Count > 0) ? _list[0] : default(T); }
		public T PeekBack() { return (_list.Count > 0) ? _list[_list.Count - 1] : default(T); }
	}
}
