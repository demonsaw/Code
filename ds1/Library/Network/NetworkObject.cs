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
	using DemonSaw.Command;
	using DemonSaw.Data;
	using DemonSaw.Http;
	using System;
	using System.Net;
	using System.Net.Sockets;
	using System.Text;

	public abstract class NetworkObject : NetworkCommand
	{
		// Constructors
		public NetworkObject() { }
		public NetworkObject(Socket socket) : base(socket) { }
		public NetworkObject(NetworkConnection connection) : base(connection) { }
		public NetworkObject(IPAddress address, int port) : base(address, port) { }
		public NetworkObject(string address, int port) : base(address, port) { }

		// TOOD: Remove
		public NetworkObject(NetworkData data) { }

		// Send
		protected void Send(HttpMessage http)
		{			
			string str = http.ToString();
			byte[] data = Encoding.ASCII.GetBytes(str);

			lock (Socket)
			{
				Lock();
				Socket.BeginSend(data, 0, data.Length, 0, new AsyncCallback(SendCallback), Socket);
				Wait();
			}
		}

		private void SendCallback(IAsyncResult ar)
		{
			try
			{
				Socket socket = (Socket) ar.AsyncState;
				socket.EndSend(ar);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
			finally
			{
				Unlock();
			}
		}

		// Send
		public void SendOk() { SendOk(new HttpResponse(HttpCode.Ok)); }
		public void SendOk(string data) { SendOk(new HttpResponse(HttpCode.Ok) { Data = data }); }
		public void SendOk(HttpResponse httpResponse) { Send(httpResponse); }

		public void SendNotFound() { SendNotFound(new HttpResponse(HttpCode.NotFound)); }
		public void SendNotFound(string data) { SendNotFound(new HttpResponse(HttpCode.NotFound) { Data = data }); }
		public void SendNotFound(HttpResponse httpResponse) { Send(httpResponse); }

		public void SendBadRequest() { SendBadRequest(new HttpResponse(HttpCode.BadRequest)); }
		public void SendBadRequest(string data) { SendBadRequest(new HttpResponse(HttpCode.BadRequest) { Data = data }); }
		public void SendBadRequest(HttpResponse httpResponse) { Send(httpResponse); }

		public void SendUnauthorized() { SendUnauthorized(new HttpResponse(HttpCode.Unauthorized)); }
		public void SendUnauthorized(string data) { SendUnauthorized(new HttpResponse(HttpCode.Unauthorized) { Data = data }); }
		public void SendUnauthorized(HttpResponse httpResponse) { Send(httpResponse); }

		public void SendInternalServerError() { SendInternalServerError(new HttpResponse(HttpCode.InternalServerError)); }
		public void SendInternalServerError(string data) { SendInternalServerError(new HttpResponse(HttpCode.InternalServerError) { Data = data }); }
		public void SendInternalServerError(HttpResponse httpResponse) { Send(httpResponse); }

		// Receive
		protected T Receive<T>() where T : HttpMessage, new()
		{
			SocketData<T> state = new SocketData<T>() { Socket = Socket };

			lock (Socket)
			{
				Lock();
				Socket.BeginReceive(state.Buffer, 0, state.BufferSize, 0, new AsyncCallback(ReceiveCallback<T>), state);
				Wait();
			}

			return (T) state.Http;
		}

		protected void Receive(out HttpRequest http)
		{
			SocketData<HttpRequest> state = new SocketData<HttpRequest>() { Socket = Socket };

			lock (Socket)
			{
				Lock();
				Socket.BeginReceive(state.Buffer, 0, state.BufferSize, 0, new AsyncCallback(ReceiveCallback<HttpRequest>), state);
				Wait();
			}

			http = (HttpRequest) state.Http;
		}

		protected void Receive(out HttpResponse http)
		{
			SocketData<HttpResponse> state = new SocketData<HttpResponse>() { Socket = Socket };

			lock (Socket)
			{
				Lock();
				Socket.BeginReceive(state.Buffer, 0, state.BufferSize, 0, new AsyncCallback(ReceiveCallback<HttpResponse>), state);
				Wait();
			}

			http = (HttpResponse) state.Http;
		}

		private void ReceiveCallback<T>(IAsyncResult ar) where T : HttpMessage, new()
		{
			try
			{
				SocketData<T> state = (SocketData<T>) ar.AsyncState;
				if (!state.Socket.Connected)
				{
					Unlock();
					return;
				}
				
				int bytesRead = state.Socket.EndReceive(ar);
				//Debug.Assert(bytesRead > 0);

				if (bytesRead > 0)
				{
					state.Http = new T();
					string str = Encoding.ASCII.GetString(state.Buffer, 0, bytesRead);
					int bytesParsed = state.Http.Parse(str);

					if (state.Http.ContentLength > 0)
					{
						int bytesRemaining = bytesRead - bytesParsed;

						if (bytesRemaining > 0)
						{
							state.ContentLength = bytesRemaining;
							string data = str.Substring(bytesParsed, bytesRemaining);

							if (bytesRemaining == state.Http.ContentLength)
							{
								state.Http.Data = data;
								Unlock();
							}
							else
							{
								state.Builder.Append(data);
								state.Socket.BeginReceive(state.Buffer, 0, state.BufferSize, 0, new AsyncCallback(ReceiveCallbackEx<T>), state);
							}
						}
						else
						{
							state.Socket.BeginReceive(state.Buffer, 0, state.BufferSize, 0, new AsyncCallback(ReceiveCallbackEx<T>), state);
						}
					}
					else
					{
						Unlock();
					}
				}
				else
				{
					Unlock();
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Unlock();
			}
		}

		private void ReceiveCallbackEx<T>(IAsyncResult ar) where T : HttpMessage, new()
		{
			try
			{
				SocketData<T> state = (SocketData<T>) ar.AsyncState;
				if (!state.Socket.Connected)
				{
					Unlock();
					return;
				}
				
				int bytesRead = state.Socket.EndReceive(ar);
				//Debug.Assert(bytesRead > 0);

				if (bytesRead > 0)
				{
					state.ContentLength += bytesRead;
					string str = Encoding.ASCII.GetString(state.Buffer, 0, bytesRead);
					state.Builder.Append(str);

					if (state.ContentLength == state.Http.ContentLength)
					{
						state.Http.Data = state.Builder.ToString();
						Unlock();
					}
					else
					{
						state.Socket.BeginReceive(state.Buffer, 0, state.BufferSize, 0, new AsyncCallback(ReceiveCallbackEx<T>), state);
					}
				}
				else
				{
					Unlock();
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Unlock();
			}
		}
	}
}
