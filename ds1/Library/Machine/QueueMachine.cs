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

namespace DemonSaw.Machine
{
	using DemonSaw.Data;
	using DemonSaw.Utility;
	using System.Collections;
	using System.Collections.Generic;
	using System.Threading;

	public abstract class QueueMachine : DemonMachine
	{
		// Properties
		protected Queue<MachineData> Queue { get; set; }
		public abstract int ActiveItems { get; }
		public abstract int MaxActiveItems { get; }

		// Properties - Helper
		protected bool Available { get { return ActiveItems < MaxActiveItems; } }

		// Constructors
		public QueueMachine() { Queue = new Queue<MachineData>(); }

		protected override void Main()
		{
			do
			{
				NextState();

				switch (State)
				{
					case DemonState.Init:
					{
						Init();
						break;
					}
					case DemonState.Process:
					{
						Process();
						break;
					}
					case DemonState.Shutdown:
					{
						Shutdown();
						break;
					}
				}

				Thread.Sleep(DemonTimeout.Milliseconds);
			}
			while (State != DemonState.None);
		}

		private void Init()
		{
			State = DemonState.Process;
		}

		private void Shutdown()
		{
			State = DemonState.None;
		}

		protected abstract void Process();

		public void Add(MachineData data)
		{
			data.Queued = true;
			data.Status = DemonStatus.None;

			lock (Queue)
				Queue.Enqueue(data);
		}

		// Utility
		public bool Contains(MachineData data) { return Queue.Contains(data); }
	}
}
