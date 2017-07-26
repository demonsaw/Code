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

namespace DemonSaw.Data
{
	using DemonSaw.Machine;
	using DemonSaw.Utility;
	using System;

	public abstract class MachineData : ItemData
	{
		// Properties
		public DemonMachine Machine { get; set; }
		public bool Queued { get; set; }

		public override TimeSpan Elapsed { get { return !Queued ? base.Elapsed : TimeSpan.Zero; } }

		// Properties - Helper
		public bool Active { get { return (Machine != null) ? Machine.Active : Queued; } }
		public bool Inactive { get { return (Machine != null) ? Machine.Inactive : !Queued; } }

		// Constructors
		public MachineData() { }
		public MachineData(string id) : base(id) { }
		public MachineData(string id, string name) : base(id, name) { }
		public MachineData(MachineData data) : base(data) { }

		// Interface
		public void Start()
		{
			Stop();

			Machine.Start();
		}

		public virtual void Start(DemonMachine machine)
		{
			Stop();

			Machine = machine;
			Machine.Start();
		}

		public virtual void Start<T>() where T : DataMachine, new()
		{
			Stop();

			Machine = new T() { Data = this };
			Machine.Start();
		}

		public virtual void Stop()
		{
			Queued = false;

			if (Machine != null)
				Machine.Stop();
		}

		public virtual void Restart()
		{
			if (Machine == null)
				return;

			Machine.Restart();
		}

		public virtual void Pause()
		{
			lock (this)
			{
				Machine.Pause();
				Status = DemonStatus.Paused;
			}
		}

		public virtual void Resume()
		{
			lock (this)
			{				
				Status = DemonStatus.Info;
				Machine.UnPause();
			}
		}
	}
}
