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
	public static class Constant
	{
		// Constants
		public const string Null = null;
		public const string Empty = "";
		public const string Newline = "\r\n";

		public const int Version = 0;
		public const string Application = "DemonSaw";
		public const string WebSite = "www.demonsaw.com";

		public const string None = "None";
		public const string Client = "Client";		

		public const string InvalidAddress = null;
		public const int InvalidPort = -1; 
		public const int InvalidIndex = -1;

		public const int MinNameLength = 3;
		public const int MinAddressLength = 7;
		public const int MinKeywordLength = 3;
		public const int MinStreamableProgress = 1;

		public const int DownloadDelay = 500;
		public const int MaxSockets = 65536;
	}
}
