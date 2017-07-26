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
	using System.Diagnostics;

	public class TransferComponent : EntityComponent
	{
		#region Variables
		private string _id;
		private long _size;
		#endregion

		#region Properties
		public string Id
		{
			get { return _id; }
			set
			{
				_id = value;
				OnPropertyChanged("Id");
			}
		}

		public long Size
		{
			get { return _size; }
			set
			{
				_size = value;
				OnPropertyChanged("Size");
				OnPropertyChanged("Speed");				
				OnPropertyChanged("Progress");
			}
		}

		public long Speed
		{
			get
			{
				EntityIdleComponent idle = Owner.Get<EntityIdleComponent>();
				return (idle.Elapsed.Seconds > 0) ? (Size / idle.Elapsed.Seconds) : Size;
			}
		}

		public TimeSpan Remaining
		{
			get
			{
				if (Size > 0)
				{
					EntityIdleComponent idle = Owner.Get<EntityIdleComponent>();
					long bytes = (Size != Size) ? (Size - Size) : 1;
					Int64 ticksPerByte = idle.Elapsed.Ticks / bytes;
					return new TimeSpan(ticksPerByte * Size);
				}

				return TimeSpan.Zero;
			}
		}

		public float Progress
		{
			get
			{
				FileComponent file = Owner.Get<FileComponent>();
				return (Size > 0) ? ((Size / (float) file.Size) * 100.0f) : 0.0f;
			}
		}

		public bool Done
		{
			get
			{				
				FileComponent file = Owner.Get<FileComponent>();
				Debug.Assert(Size <= file.Size);

				return Size >= file.Size;
			}
		}
		#endregion

		#region Interface
		public override void Update()
		{
			base.Update();

			OnPropertyChanged("Remaining");
		}
		#endregion
	}
}
