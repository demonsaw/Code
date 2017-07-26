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
	using DemonSaw.Utility;
	using System;

	public abstract class TransferData : FileData
	{
		// Constants
		public const string XmlRoot = "transfer";

		// Variables
		private long _remaining;

		// Properties
		public ClientData Client { get; set; }

		public override DateTime Updated
		{
			get { return base.Updated; }
			set
			{
				base.Updated = value;
				OnPropertyChanged("RemainingTime");
				OnPropertyChanged("Speed");
			}
		}

		// Properties - Helper
		public long Speed { get { return (Elapsed.Seconds > 0) ? (TransferedBytes / Elapsed.Seconds) : TransferedBytes; } }
		public long TransferedBytes { get { return Size - RemainingBytes; } }

		public long RemainingBytes
		{
			get { return _remaining; }
			set
			{
				_remaining = value;
				OnPropertyChanged("Progress");
			}
		}

		public TimeSpan RemainingTime
		{ 
			get 
			{
				if (TransferedBytes > 0)
				{
					long bytes = (Size != RemainingBytes) ? (Size - RemainingBytes) : 1;
					Int64 ticksPerByte = Elapsed.Ticks / bytes;
					return new TimeSpan(ticksPerByte * RemainingBytes);
				}

				return TimeSpan.Zero;
			} 
		}

		public virtual bool CanOpen { get { return true; } }
		public bool CanCancel { get { return Active; } }
		public bool CanPause { get { return !Paused && (Active && !Queued); } }
		public bool CanResume { get { return Paused; } }

		public float Progress { get { return (Size > 0) ? (((Size - RemainingBytes) / (float) Size) * 100.0f) : 100.0f; } }

		public RemoteRouterData Router { get { return Client.Router; } }
		public bool Finished { get { return RemainingBytes == 0; } }

		// Constructors
		public TransferData(FileData file, ClientData client) : base(file)
		{
			Enabled = true;
			Client = client;
			RemainingBytes = file.Size;
			Created = Updated = DateTime.Now;
		}

		// Interface
		public override void Restart()
		{ 
			RemainingBytes = Size;
			Start();
		}

		// Utility
		public virtual void Cancel()
		{
			Status = DemonStatus.Cancelled;			
			Stop();
		}
	}
}
