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

namespace DemonSaw.Command
{
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.Diagnostics;
	using System.Net;
	using System.Net.Sockets;

	public abstract class NetworkCommand : DemonEvent, IDisposable
	{
		#region Variables
		private NetworkConnection _connection = new NetworkConnection(); 
		#endregion

		#region Properties
		public NetworkConnection Connection { get { return _connection; } }
		protected Socket Socket { get { return _connection.Socket; } }
		protected bool Owner { get { return Connection.Owner; } }

		public bool Connected { get { return _connection.Connected; } }
		public string Address { get { return _connection.Address; } }
		public int Port { get { return _connection.Port; } }
		#endregion

		#region Constructors
		protected NetworkCommand() { }

		protected NetworkCommand(Socket socket) 
		{ 
			Init(socket); 
		}

		protected NetworkCommand(NetworkConnection connection) 
		{ 
			Init(connection); 
		}

		protected NetworkCommand(IPAddress address, int port) 
		{ 
			Init(address, port); 

		}
		protected NetworkCommand(string address, int port) 
		{ 
			Init(address, port); 
		}

		~NetworkCommand() 
		{ 
			Shutdown(); 
		}
		#endregion

		#region System
		void IDisposable.Dispose() { Shutdown(); }
		#endregion

		#region Interface		
		public void Init(Socket socket)
		{
			Debug.Assert(socket != null && socket.Connected);

			_connection.Start(socket);
		}

		public void Init(NetworkConnection connection)
		{
			Debug.Assert(connection != null && connection.Connected);

			_connection.Start(connection);
		}

		public void Init(IPAddress address, int port)
		{
			Debug.Assert(address != null && port > 0);

			_connection.Start(address, port);
		}

		public void Init(string address, int port)
		{
			Debug.Assert(!string.IsNullOrEmpty(address) && port > 0);

			_connection.Start(address, port);
		}

		public virtual void Update() { }

		public virtual void Shutdown()
		{
			_connection.Stop();
		}

		public virtual void Clear() { }
		#endregion
	}
}
