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
	using System.Collections.Generic;
	using System.Diagnostics;

	public class MapComponent<Key, Value> : EntityComponent
	{
		// Variables
		protected Dictionary<Key, Value> _map = new Dictionary<Key, Value>();

		// Properties - Helper
		public int Count { get { return _map.Count; } }
		public bool Empty { get { return _map.Count <= 0; } }

		public Dictionary<Key, Value>.KeyCollection Keys { get { return _map.Keys; } }
		public Dictionary<Key, Value>.ValueCollection Values { get { return _map.Values; } }
		public Dictionary<Key, Value> Map { get { return _map; } }

		#region Container
		public virtual void Add(Key key, Value value)
		{
			Debug.Assert(key != null && value != null);

			lock (_map)
			{
				if (!_map.ContainsKey(key))
					_map.Add(key, value);
				else
					_map[key] = value;
			}				
		}

		public virtual bool Remove(Key key)
		{
			Debug.Assert(key != null);

			lock (_map)
				return _map.Remove(key);
		}

		public virtual void Clear()
		{
			lock (_map)
				_map.Clear();
		}

		public bool Contains(Key key)
		{
			Debug.Assert(key != null);

			lock (_map)
				return _map.ContainsKey(key);
		}

		public Value Get(Key key)
		{
			Debug.Assert(key != null);

			Value value;
			lock (_map)
				_map.TryGetValue(key, out value);

			return value;
		}
		#endregion
	}
}
