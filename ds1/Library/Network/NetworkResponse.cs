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

namespace DemonSaw.Network
{
	using DemonSaw.Data;
	using DemonSaw.Http;
	using System.Net;
	using System.Net.Sockets;

	public class NetworkResponse : NetworkObject
	{
		// Constructors
		public NetworkResponse() { }
		public NetworkResponse(Socket socket) : base(socket) { }
		public NetworkResponse(NetworkConnection connection) : base(connection) { }
		public NetworkResponse(IPAddress address, int port) : base(address, port) { }
		public NetworkResponse(string address, int port) : base(address, port) { }
		
		// TOOD: Remove
		public NetworkResponse(NetworkData data) { }

		// Send
		public void Send(HttpResponse httpResponse)
		{
			base.Send(httpResponse);
		}

		// Receive
		public HttpResponse Receive()
		{
			return Receive<HttpResponse>();
		}

		public new void Receive(out HttpResponse httpResponse)
		{
			base.Receive(out httpResponse);
		}
	}
}
