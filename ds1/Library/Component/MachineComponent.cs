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
	using DemonSaw.Collection;
	using DemonSaw.Entity;
	using DemonSaw.Utility;
	using System;
	using System.Diagnostics;
	using System.Threading;

	public abstract class MachineComponent : EntityComponent, IDisposable
	{
		// Variables
		private DemonState _state = DemonState.None; 
		private DemonStatus _status = DemonStatus.None;
	
		// Properties
		public bool Paused { get; private set; }
		private Deque<DemonState> States { get; set; }
		private DemonState SleepState { get; set; }

		private ManualResetEvent Event { get; set; }
		private Timer SleepTimer { get; set; }

		// Properties - Helper
		public bool Active { get; private set; }
		public bool Inactive { get { return !Active; } }

		public DemonStatus Status
		{
			get { return _status; }
			set
			{
				_status = value;
				OnPropertyChanged("Status");
			}
		}

		public bool None { get { return Status == DemonStatus.None; } }
		public bool Info { get { return Status == DemonStatus.Info; } }
		//public bool Paused { get { return Status == DemonStatus.Paused; } }
		public bool Success { get { return Status == DemonStatus.Success; } }
		public bool Warning { get { return Status == DemonStatus.Warning; } }
		public bool Error { get { return Status == DemonStatus.Error; } }
		public bool Cancelled { get { return Status == DemonStatus.Cancelled; } }

		public DemonState State
		{
			get { return _state; }
			set
			{
				lock (States)
				{
					switch (value)
					{
						case DemonState.None:
						{
							States.PushFront(value);
							break;
						}
						default:
						{
							States.PushBack(value);
							break;
						}
					}
				}
			}
		}

		// Constructors
		protected MachineComponent()
		{
			States = new Deque<DemonState>();
			SleepState = DemonState.None;

			Event = new ManualResetEvent(false);
			SleepTimer = new Timer(SleepTimeout, null, Timeout.InfiniteTimeSpan, Timeout.InfiniteTimeSpan);
		}

		#region System
		void IDisposable.Dispose()
		{
			if (SleepTimer != null)
			{
				lock (SleepTimer)
				{
					SleepTimer.Dispose();
					SleepTimer = null;
				}
			}
		}
		#endregion

		#region Interface
		public override void Shutdown()
		{
			base.Shutdown();

			Stop();
		}

		public virtual void Start()
		{
			Debug.Assert(Inactive);

			try
			{
				Active = true; 
				States.Clear();
				State = DemonState.Init;
				Status = DemonStatus.Info;
				
				Thread thread = new Thread(new ThreadStart(Main)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
				thread.Start();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}	
		}
		
		public virtual void Stop()
		{
			State = DemonState.Shutdown;
			SleepState = DemonState.None;
			Status = DemonStatus.None;
			Active = false;
		}

		public virtual void Restart()
		{
			State = DemonState.Restart;
			Status = DemonStatus.None;
		}

		public void Pause() { Paused = true; }
		public virtual void UnPause() { Paused = false; }
		#endregion

		#region Utility
		protected void Lock() { Event.Reset(); }
		protected void Unlock() { Event.Set(); }
		protected void Wait() { Event.WaitOne(); }
		#endregion

		#region Machine
		protected abstract void Main();

		protected void NextState()
		{
			lock (States)
			{
				if (States.Count > 0)
					_state = States.Pop();
			}
		}

		protected void RestartState()
		{
			Sleep(DemonTimeout.Restart, DemonState.Init);
		}

		protected void Sleep() { Sleep(DemonTimeout.Sleep); }
		protected void Sleep(TimeSpan time) { Sleep(time, State); }
		protected void Sleep(DemonState state) { Sleep(DemonTimeout.Sleep, state); }
		protected virtual void Sleep(TimeSpan time, DemonState state)
		{
			if (SleepState == DemonState.None)
			{
				SleepState = state;
				State = DemonState.Sleep;

				if (SleepTimer != null)
				{
					lock (SleepTimer)
						SleepTimer.Change(time, Timeout.InfiniteTimeSpan);
				}					
			}
		}

		private void SleepTimeout(object obj)
		{
			if (SleepState != DemonState.None)
			{
				State = SleepState;
				SleepState = DemonState.None;
			}
		}
		#endregion
	}
}
