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

	public class EntityMapComponent : EntityComponent
	{
		#region Variables
		private EntityMap _map;
		#endregion

		#region Properties
		public Dictionary<string, Entity> Map { get { return _map.Map; } }
		public bool Empty { get { return _map.Empty; } }
		public int Count { get { return _map.Count; } }

		public override bool Valid { get { return base.Valid && _map.Valid; } }
		#endregion

		#region Constructors
		public EntityMapComponent() { _map = new EntityMap(); }
		#endregion

		#region Interface
		public override void Update()
		{
			base.Update();

			_map.Update();
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

			_map.Add(entity);
		}

		public void Add(string id, Entity entity)
		{
			Debug.Assert(entity != Owner);

			_map.Add(id, entity);
		}

		public bool Remove(string id)
		{
			return _map.Remove(id);
		}

		public bool Remove(Entity entity)
		{
			Debug.Assert(entity != Owner);

			return _map.Remove(entity);
		}

		public void Clear()
		{
			_map.Clear();
		}

		public bool Contains(string id)
		{
			return _map.Contains(id);
		}

		public bool Contains(Entity entity)
		{
			Debug.Assert(entity != Owner);

			return _map.Contains(entity);
		}

		public Entity Get(string id)
		{
			return _map.Get(id);
		}
		#endregion
	}
}
