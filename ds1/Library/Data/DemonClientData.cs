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

	public abstract class DemonClientData : NetworkData
	{
		// Constants
		public const string XmlRoot = "client";

		// Properties
		public SessionData Session { get; set; }

		// Constructors
		protected DemonClientData() { Init(); }
		protected DemonClientData(string id) : base(id) { Init(); }
		protected DemonClientData(string id, string name) : base(id, name) { Init(); }
		protected DemonClientData(DemonClientData data) : base(data) { Init(); }

		// Interface
		private void Init()
		{
			Session = new SessionData();
		}

		public override void Clear()
		{
			base.Clear();

			Session.Clear();
			Stop();
		}

		// Utility
		public override bool Idle
		{
			get
			{
				TimeSpan elapsed = DateTime.Now - Updated;
				return elapsed > DemonTimeout.Client;
			}
		}
	}
}
