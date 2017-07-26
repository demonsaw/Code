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
	using System.Diagnostics;

	public sealed class EntityStoreComponent : EntityComponent, EntityIdle
	{
		#region Variables
		private Entity _entity;
		#endregion

		#region Properties
		public bool Empty { get { return _entity == null; } }
		public override bool Valid { get { return base.Valid && ((_entity != null) ? _entity.Valid : false); } }

		public Entity Entity
		{
			get { return _entity; }
			set
			{
				if (_entity != value)
				{
					Clear();

					if (value != null)
						Add(value);
				}
			}
		}
		#endregion

		#region Constructors
		public EntityStoreComponent() { }
		public EntityStoreComponent(Entity entity) { Add(entity); }
		#endregion

		#region Interface
		public override void Update()
		{
			base.Update();

			_entity.Update();
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
			Debug.Assert(entity != null);

			EntityIdleComponent idle = entity.Get<EntityIdleComponent>();
			if (idle != null)
				idle.Add(this);

			entity++;

			_entity = entity;
		}

		public bool Remove(Entity entity)
		{
			Debug.Assert(entity != null && entity == _entity);

			Clear();

			return true;
		}

		public void Clear()
		{
			if (_entity != null)
			{
				EntityIdleComponent idle = _entity.Get<EntityIdleComponent>();
				if (idle != null)
					idle.Remove(this);

				_entity--;
				_entity = null;
			}
		}

		public T Get<T>() where T : EntityComponent
		{
			return Entity.Get<T>();
		}

		public bool Contains<T>() where T : EntityComponent
		{
			return Entity.Contains<T>();
		}
		#endregion
	}
}
