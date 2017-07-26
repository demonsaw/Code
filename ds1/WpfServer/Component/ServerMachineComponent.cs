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
	using DemonSaw.Command.Response;
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.Diagnostics;
	using System.Net;
	using System.Net.Sockets;
	using System.Threading;

	public sealed class ServerMachineComponent : MachineComponent, IDisposable
	{
		// Properties
		private Socket Socket { get; set; }

		// Properties - Helper
		public bool Connected { get { return Socket != null && Socket.Connected; } }

		#region System
		void IDisposable.Dispose()
		{
			ShutdownState();
		}
		#endregion

		#region Interface
		public override void Init()
		{
			base.Init();

			ServerComponent server = Entity.Get<ServerComponent>();
			if (server.Enabled && Inactive)
				Start();
		}

		public override void Stop()
		{
			base.Stop();

			Disconnect();
		}

		public override void Restart()
		{
			base.Restart();

			Disconnect();
		}
		#endregion

		#region Utility
		private bool Connect()
		{
			if (Connected)
				return false;

			try
			{
				ServerComponent server = Entity.Get<ServerComponent>();
				IPAddress address = IPAddress.Parse(server.Address);
				IPEndPoint endpoint = new IPEndPoint(address, server.Port);

				Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				Socket.Bind(endpoint);
				Socket.Listen(Constant.MaxSockets);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				return false;
			}

			return true;
		}

		private bool Disconnect()
		{
			try
			{
				if (Socket != null)
				{
					if (Socket.Connected)
						Socket.Shutdown(SocketShutdown.Both);

					Socket.Close();
					Socket = null;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				return false;
			}

			return true;
		}
		#endregion

		#region Machine
		protected override void Main()
		{
			do
			{
				NextState();

				switch (State)
				{
					case DemonState.Init:
					{
						InitState();
						break;
					}
					case DemonState.Listen:
					{
						ListenState();
						break;
					}
					case DemonState.Restart:
					{
						RestartState();
						break;
					}
					case DemonState.Shutdown:
					{
						ShutdownState();
						break;
					}
				}

				Thread.Sleep(DemonTimeout.None);
			}
			while (State != DemonState.None);
		}

		private void InitState()
		{
			try
			{
				if (Entity.Invalid)
				{
					State = DemonState.Restart;
					Status = DemonStatus.Warning;					
					return;
				}

				if (!Connect())
				{
					State = DemonState.Restart;
					Status = DemonStatus.Error;					
					return;
				}

				State = DemonState.Listen;
				Status = DemonStatus.Success;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart; 
				Status = DemonStatus.Error;
			}
		}

		private void ShutdownState()
		{
			Disconnect();

			State = DemonState.None;
			Status = DemonStatus.None;
		}

		private void ListenState()
		{
			try
			{
				Lock();
				//Console.WriteLine("Waiting for a socket...");
				Socket.BeginAccept(new AsyncCallback(AcceptState), Socket);
				Wait();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private void AcceptState(IAsyncResult ar)
		{
			try
			{
				Unlock();
				Socket listener = (Socket) ar.AsyncState;
				Socket socket = listener.EndAccept(ar);
				Debug.Assert(socket != null);

				Thread thread = new Thread(new ParameterizedThreadStart(ProcessState)) { Priority = ThreadPriority.Normal, IsBackground = true };
				thread.Start(socket);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private void ProcessState(object obj)
		{
			Socket socket = (Socket) obj;
			NetworkChannel channel = null;

			JsonAction action = JsonAction.None;
			JsonType jsonType = JsonType.None;

			try
			{
				do
				{
					// Connect				
					channel = new NetworkChannel(socket);

					// Receive	
					HttpRequest httpRequest;
					channel.Receive(out httpRequest);
					if (httpRequest == null)
					{
						Status = DemonStatus.Warning;
						return;
					}

					// Data
					Status = DemonStatus.Success;

					// Handshake
					if (httpRequest.Session == null)
					{
						ServerStatusComponent status = Entity.Get<ServerStatusComponent>();
						HandshakeResponseCommand command = new HandshakeResponseCommand(Entity, socket) { Listener = status.Update };
						command.Execute(httpRequest);
						break;
					}

					// Session
					SessionMapComponent sessions = Entity.Get<SessionMapComponent>();
					Entity entity = sessions.Get(httpRequest.Session);
					if (entity == null)
					{
						channel.SendUnauthorized();
						break;
					}

					// TODO: Version check
					//
					// Message
					SessionComponent session = entity.Get<SessionComponent>();
					string decrypted = session.Decrypt(httpRequest.Data);
					JsonPacket jsonRequest = JsonPacket.Parse(decrypted);
					JsonMessage jsonRequestMessage = JsonMessage.Parse(jsonRequest.Message);
					jsonType = jsonRequestMessage.Type;

					switch (jsonType)
					{
						case JsonType.Handshake:
						{
							HandshakeResponseCommand command = new HandshakeResponseCommand(Entity, socket);
							command.Execute(httpRequest, jsonRequest, session);
							break;
						}
						case JsonType.Ping:
						{
							PingResponseCommand command = new PingResponseCommand(Entity, socket);
							command.Execute(httpRequest, jsonRequest, session);
							break;
						}
						case JsonType.Info:
						{
							InfoResponseCommand command = new InfoResponseCommand(Entity, socket);
							command.Execute(httpRequest, jsonRequest, session);
							break;
						}
						case JsonType.Download:
						{
							// TODO: Add max transfer check!
							//
							//ServerDownloadCommand command = new ServerDownloadCommand(Entity, socket);
							//action = command.Execute(httpRequest, jsonRequest, session);
							channel.SendBadRequest();
							break;
						}
						case JsonType.Upload:
						{
							//ServerUploadCommand command = new ServerUploadCommand(Entity, socket);
							//action = command.Execute(httpRequest, jsonRequest, session);
							channel.SendBadRequest();
							break;
						}
						case JsonType.Quit:
						{
							QuitResponseCommand command = new QuitResponseCommand(Entity, socket);
							command.Execute(httpRequest, jsonRequest, session);
							break;
						}
						default:
						{
							channel.SendBadRequest();
							return;
						}
					}
				}
				while (action != JsonAction.None);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Status = DemonStatus.Error;
				channel.SendInternalServerError();
			}
			finally
			{
				if (channel != null)
					channel.Shutdown();

				socket.Shutdown(SocketShutdown.Both);
				socket.Close();
			}
		}
		#endregion
	}
}
