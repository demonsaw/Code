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
	using DemonSaw.Utility;
	using System;
	using System.Diagnostics;
	using System.Net;
	using System.Net.Sockets;

	public sealed class NetworkConnection : DemonEvent, IDisposable
	{
		// Properties
		public Socket Socket { get; set; }
		public bool Owner { get; set; }

		// Properties - Helper
		//public bool Connected { get { return Socket != null && Socket.Connected; } }
		public bool Connected
		{
			get
			{
				if (Socket != null && Socket.Connected)
					return !(Socket.Poll(1, SelectMode.SelectRead) && Socket.Available == 0);

				return false;
			}
		}

		public string Address { get { return NetworkUtil.GetAddress(Socket.RemoteEndPoint); } }
		public int Port { get { return NetworkUtil.GetPort(Socket.RemoteEndPoint); } }		

		// Constructors
		public NetworkConnection() { }
		public NetworkConnection(NetworkConnection connection) { Start(connection); }
		public NetworkConnection(Socket socket) { Start(socket); }
		public NetworkConnection(string address, int port) { Start(address, port); }
		public NetworkConnection(IPAddress address, int port) { Start(address, port); }
		public NetworkConnection(NetworkData data) { Start(data.Address, data.Port); }		

		~NetworkConnection() { Stop(); }

		// Overrides
		void IDisposable.Dispose() { Stop(); }

		// Utility
		public void Start(NetworkConnection connection)
		{
			Debug.Assert(connection != null);

			Start(connection.Socket); 
		}

		public void Start(Socket socket)
		{
			Debug.Assert(socket != null && socket.Connected);

			Stop();

			Owner = false;
			Socket = socket;

			if (Socket == null || !Socket.Connected)
			{
				Exception = new Exception("Invalid socket");
				throw Exception;
			}
		}

		public void Start(NetworkData data)
		{
			Debug.Assert(data != null);

			Start(data.Address, data.Port); 
		}

		public void Start(string address, int port)
		{
			Debug.Assert(!string.IsNullOrEmpty(address) && port > 0);

			Start(NetworkUtil.GetIPAddress(address), port); 
		}

		public void Start(IPAddress address, int port)
		{
			Debug.Assert(address != null && port > 0);

			Stop();			
			
			Owner = true;
			IPEndPoint endpoint = new IPEndPoint(address, port);
			Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

			Lock();			
			Socket.BeginConnect(endpoint, new AsyncCallback(Callback), Socket);
			Wait();

			if (Exception != null)
				throw Exception;
		}

		private void Callback(IAsyncResult ar)
		{
			try
			{
				Socket socket = (Socket) ar.AsyncState;
				socket.EndConnect(ar);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Exception = ex;
			}
			finally
			{
				Unlock();
			}
		}

		public void Stop()
		{
			try
			{
				if (Owner)
				{
					Owner = false;

					if (Socket != null)
					{
						if (Socket.Connected)
							Socket.Shutdown(SocketShutdown.Both);

						Socket.Close();
						Socket = null;	
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
			finally
			{
				Exception = null;
				if (Socket != null && !Socket.Connected)
					Socket = null;
			}
		}
	}
}
