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
	using DemonSaw.Controller;
	using DemonSaw.Entity;
	using DemonSaw.Http;

#if DEBUG
	public sealed class LogComponent : EntityComponent
	{
		// Properties
		public LogController Controller { get; private set; }

		// Constructors
		public LogComponent(LogController controller) { Controller = controller; }

		// Utility
		public void Add(object obj)
		{
			Controller.Add(obj);
		}

		public void Add(HttpRequest httpRequest, object objRequest = null)
		{
			Controller.Add(httpRequest, objRequest);
		}

		public void Add(HttpResponse httpResponse, object objResponse = null)
		{
			Controller.Add(httpResponse, objResponse);
		}

		public void Add(HttpRequest httpRequest, HttpResponse httpResponse, object objRequest = null, object objResponse = null)
		{
			Controller.Add(httpRequest, httpResponse, objRequest, objResponse);
		}
	}
#endif
}
