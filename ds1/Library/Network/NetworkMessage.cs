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
	using System;
	using System.Net;
	using System.Net.Sockets;
	using System.Threading;

	public sealed class NetworkMessage : NetworkObject
	{
		// Properties
		public object Data { get; set; }

		// Constructors
		public NetworkMessage(object obj = null) { Data = obj; }
		public NetworkMessage(Socket socket, object obj = null) : base(socket) { Data = obj; }
		public NetworkMessage(NetworkConnection connection, object obj = null) : base(connection) { Data = obj; }
		public NetworkMessage(IPAddress address, int port, object obj = null) : base(address, port) { Data = obj; }
		public NetworkMessage(string address, int port, object obj = null) : base(address, port) { Data = obj; }

		// TOOD: Remove
		public NetworkMessage(NetworkData data, object obj = null) { Data = obj; }

		// Send
		public void Send(HttpRequest httpRequest)
		{
			httpRequest.Host = NetworkUtil.GetAddress(Connection.Socket.RemoteEndPoint);

			Thread thread = new Thread(new ParameterizedThreadStart(RequestThread)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
			thread.Start(httpRequest);
		}

		// Utility
		private void RequestThread(object obj)
		{
			try
			{
				Lock();

				HttpRequest httpRequest = (HttpRequest) obj;
				HttpResponse httpResponse;

				if (Connection.Owner)
				{
					base.Send(httpRequest);
					//OnRequest(httpRequest, Data);

					httpResponse = base.Receive<HttpResponse>();
					//OnResponse(httpResponse, Data);
				}
				else
				{
					lock (Connection.Socket)
					{
						base.Send(httpRequest);
						//OnRequest(httpRequest, Data);

						httpResponse = base.Receive<HttpResponse>();
						//OnResponse(httpResponse, Data);
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
			finally
			{
				Shutdown();
				Unlock();
			}
		}
	}
}
