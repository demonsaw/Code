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

namespace DemonSaw.Utility
{
	using System;
	using System.Threading;

	//Taken From: http://www.interact-sw.co.uk/iangblog/2004/03/23/locking
	//
	// Thanks to Eric Gunnerson for recommending this be a struct rather than a class - avoids a heap allocation.
	// (In Debug mode, we make it a class so that we can add a finalizer in order to detect when the object is not freed.)
	// Thanks to Chance Gillespie and Jocelyn Coulmance for pointing out the bugs that then crept in when I changed it to use struct...
	public class LockTimeoutException : Exception
	{
		public LockTimeoutException() : base("Timeout waiting for lock") { }
	}

#if DEBUG
	public class TimedLock : IDisposable
#else
	public struct TimedLock : IDisposable
#endif
	{
		// Variables
		private const int _default = 5;
		private object _target;

		// Constructors
		private TimedLock(object obj) { _target = obj; }

#if DEBUG
		~TimedLock()
		{
			// If this finalizer runs, someone somewhere failed to call Dispose, which means we've failed to leave a monitor!
			System.Diagnostics.Debug.Fail("Undisposed lock");
		}
#endif

		public void Dispose()
		{
			Monitor.Exit(_target);

			// It's a bad error if someone forgets to call Dispose, so in Debug builds, we put a finalizer in to detect
			// the error. If Dispose is called, we suppress the finalizer.
#if DEBUG
			GC.SuppressFinalize(this);
#endif
		}

		// Static
		public static TimedLock Lock(object obj)
		{
			return Lock(obj, TimeSpan.FromSeconds(_default));
		}

		public static TimedLock Lock(object obj, TimeSpan timeout)
		{
			TimedLock tl = new TimedLock(obj);

			if (!Monitor.TryEnter(obj, timeout))
			{
#if DEBUG
				System.GC.SuppressFinalize(tl);
#endif
				throw new LockTimeoutException();
			}

			return tl;
		}

	}
}
