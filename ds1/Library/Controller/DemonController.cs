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

namespace DemonSaw.Controller
{
	using DemonSaw.Utility;
	using System;
	using System.Threading;
	using System.Windows.Controls;

	public abstract class DemonController : IState
	{
		// Properties
		protected TextBlock Title { get; set; }
		protected Timer CursorTimer { get; set; }

		// Constructors
		protected DemonController()
		{
			CursorTimer = new Timer(delegate { Stop(); }, null, Timeout.InfiniteTimeSpan, Timeout.InfiniteTimeSpan);
		}

		protected DemonController(TextBlock title) : this()
		{ 
			Title = title; 
		}

		#region Interface
		public virtual void Init() { }

		public virtual void Update() { }

		public virtual void Shutdown()
		{
			try
			{
				if (CursorTimer != null)
				{
					CursorTimer.Dispose();
					CursorTimer = null;
				}
			}
			catch (Exception) { }
		}

		public virtual void Clear() { }
		#endregion

		#region Utility
		public virtual void Start()
		{
			Start(DemonTimeout.Cursor);
		}

		public virtual void Start(TimeSpan time)
		{
			Cursor.Wait();

			try
			{
				if (CursorTimer != null)
					CursorTimer.Change(time, Timeout.InfiniteTimeSpan);
			}
			catch (Exception) { }
		}

		public virtual void Stop()
		{
			try
			{
				if (CursorTimer != null)
					CursorTimer.Change(Timeout.InfiniteTimeSpan, Timeout.InfiniteTimeSpan);
			}
			catch (Exception) { }

			Cursor.Normal();
		}
		#endregion
	}
}
