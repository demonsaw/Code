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

	public abstract class DemonRouterData : NetworkData
	{
		// Constants
		public const string XmlRoot = "router";

		// Variables
		protected string _passphrase;		

		// Properties - Helper
		public string Passphrase
		{
			get { return _passphrase; }
			set
			{
				_passphrase = value;
				OnPropertyChanged("Passphrase");
			}
		}

		// Constructors
		protected DemonRouterData() { Init(); }
		protected DemonRouterData(string id) : base(id) { Init(); }
		protected DemonRouterData(string id, string name) : base(id, name) { Init(); }
		protected DemonRouterData(DemonRouterData data) : base(data) { Init(); }

		// Utility
		private void Init()
		{
			Passphrase = Default.Passphrase;
		}

		public override void Clear()
		{
			base.Clear();

			Passphrase = Default.Passphrase;
		}
	}
}
