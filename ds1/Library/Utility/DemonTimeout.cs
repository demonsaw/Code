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

	public static class DemonTimeout
	{
		// Data
		public static readonly TimeSpan Default = TimeSpan.MaxValue;		
		public static readonly TimeSpan None = TimeSpan.FromMilliseconds(0);
		public static readonly TimeSpan Cursor = TimeSpan.FromSeconds(3);

		public static readonly TimeSpan Idle = TimeSpan.FromMinutes(15);
		public static readonly TimeSpan IdleOld = TimeSpan.FromMinutes(1);
		public static readonly TimeSpan Sleep = TimeSpan.FromMilliseconds(100);		
		public static readonly TimeSpan Restart = TimeSpan.FromSeconds(1);

		public static readonly TimeSpan Entity = TimeSpan.FromMinutes(1);
		public static readonly TimeSpan Client = TimeSpan.FromMinutes(10);
		public static readonly TimeSpan File = TimeSpan.FromSeconds(1);

		public static readonly TimeSpan Ping = TimeSpan.FromMinutes(3); 
		public static readonly TimeSpan Download = TimeSpan.FromMinutes(60);
		public static readonly TimeSpan Upload = TimeSpan.FromMinutes(15);

		// Generic
		private const int Multiplier = 5;

		public static readonly TimeSpan Millisecond = TimeSpan.FromMilliseconds(1);
		public static readonly TimeSpan Milliseconds = TimeSpan.FromMilliseconds(Multiplier);

		public static readonly TimeSpan Second = TimeSpan.FromSeconds(1);
		public static readonly TimeSpan Seconds = TimeSpan.FromSeconds(Multiplier);

		public static readonly TimeSpan Minute = TimeSpan.FromMinutes(1);
		public static readonly TimeSpan Minutes = TimeSpan.FromMinutes(Multiplier);
	}
}
