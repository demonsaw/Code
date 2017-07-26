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

	public class NetworkChannel : NetworkObject
	{
		// Constructors
		public NetworkChannel(Socket socket) { Init(socket); }
		public NetworkChannel(NetworkConnection connection) { Init(connection); }
		public NetworkChannel(IPAddress address, int port) { Init(address, port); }
		public NetworkChannel(string address, int port) { Init(address, port); }

		// TOOD: Remove
		public NetworkChannel(NetworkData data) { }

		// Send
		public void Send(HttpRequest httpRequest)
		{
			httpRequest.Host = NetworkUtil.GetAddress(Socket.RemoteEndPoint);

			base.Send(httpRequest);
		}

		public void Send(HttpResponse httpResponse)
		{
			base.Send(httpResponse);
		}

		// Receive
		public new void Receive(out HttpRequest httpRequest)
		{
			base.Receive(out httpRequest);
		}

		public new void Receive(out HttpResponse httpResponse)
		{
			base.Receive(out httpResponse);
		}
	}
}
