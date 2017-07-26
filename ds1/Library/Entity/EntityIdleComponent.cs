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
	using DemonSaw.Utility;
	using System;
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.Threading;

	public sealed class EntityIdleComponent : EntityComponent
	{
		#region Variables
		private DateTime _created;
		private DateTime _updated;
		private List<EntityIdle> _list = new List<EntityIdle>();
		#endregion

		#region Properties
		private TimeSpan Period { get; set; }
		private Timer IdleTimer { get; set; }

		public DateTime Created
		{
			get { return _created; }
			set
			{
				_created = _updated = value;
				OnPropertyChanged("Created");
				OnPropertyChanged("Updated");
				OnPropertyChanged("Elapsed");
			}
		}

		public DateTime Updated
		{
			get { return _updated; }
			set
			{
				_updated = value;
				OnPropertyChanged("Updated");
				OnPropertyChanged("Elapsed");
			}
		}
		
		public bool Idle { get { return Elapsed > DemonTimeout.Idle; } }
		public TimeSpan Elapsed { get { return DateTime.Now - Created; } }

		public int Count { get { return _list.Count; } }
		public bool Empty { get { return _list.Count <= 0; } }
		#endregion

		#region Constructors
		public EntityIdleComponent() : this(DemonTimeout.Idle) { }

		public EntityIdleComponent(TimeSpan period)
		{
			Period = period;
			_created = _updated = DateTime.Now;
			IdleTimer = new Timer(IdleTimeout, null, period, Timeout.InfiniteTimeSpan);
		}
		#endregion

		#region Interface
		public override void Update()
		{
			base.Update();

			Updated = DateTime.Now;
			if (IdleTimer != null)
			{
				lock (IdleTimer)
					IdleTimer.Change(Period, Timeout.InfiniteTimeSpan);
			}
		}

		public override void Shutdown()
		{
			lock (_list)
			{
				List<EntityIdle> list = new List<EntityIdle>(_list);

				foreach (EntityIdle idle in list)
					idle.Remove(Owner);

				Debug.Assert(Empty);
			}

			if (IdleTimer != null)
			{
				lock (IdleTimer)
				{
					IdleTimer.Dispose();
					IdleTimer = null;
				}
			}

			base.Shutdown();
		}
		#endregion

		#region Container
		public void Add(EntityIdle idle)
		{
			Debug.Assert(idle != null);

			lock (_list)
				_list.Add(idle);
		}

		public bool Remove(EntityIdle idle)
		{
			Debug.Assert(idle != null);

			lock (_list)
				return _list.Remove(idle);
		}

		public void Clear()
		{
			_list.Clear();
		}

		public bool Contains(EntityIdle idle)
		{
			Debug.Assert(idle != null);

			lock (_list)
				return _list.Contains(idle);
		}
		#endregion

		#region Utility
		private void IdleTimeout(object obj)
		{
			Owner.Shutdown();
		}
		#endregion
	}
}
