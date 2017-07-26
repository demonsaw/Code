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
	using DemonSaw.Command.Request;
	using DemonSaw.Command.Response;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.Collections.Generic;
	using System.Threading;

	// Delegates
	public delegate void SearchListEventHandler(List<FileComponent> list);

	public sealed class ClientMachineComponent : MachineComponent, IDisposable
	{
		#region Variables
		private NetworkConnection _connection;
		#endregion

		#region Handlers
		public SearchListEventHandler ListAdded;
		#endregion

		#region Properties
		private NetworkConnection Connection { get { return _connection; } }
		private bool Connected { get { return (_connection != null) ? _connection.Connected : false; } }
		#endregion

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

			ClientComponent client = Owner.Get<ClientComponent>();
			if (client.Enabled && Inactive)
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

		#region Events
		private void OnListAdded(List<FileComponent> list)
		{
			if (ListAdded != null)
				ListAdded(list);
		}
		#endregion

		#region Utility
		private void Connect()
		{
			Disconnect();

			ServerComponent server = Owner.Get<ServerComponent>();
			_connection = new NetworkConnection(server.Address, server.Port);
		}

		private void Disconnect()
		{
			if (Connection != null)
			{
				Connection.Stop();
				_connection = null;
			}
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
					case DemonState.Handshake:
					{
						HandshakeState();
						break;
					}
					case DemonState.Join:
					{
						JoinState();
						break;
					}
					case DemonState.Tunnel:
					{
						TunnelState();
						break;
					}
					case DemonState.Listen:
					{
						ListenState();
						break;
					}
					case DemonState.Quit:
					{
						QuitState();
						break;
					}
					case DemonState.Restart:
					{
						Disconnect();
						Sleep(DemonTimeout.Restart, DemonState.Init);
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
		#endregion

		#region Init
		private void InitState()
		{
			try
			{
				if (Owner.Invalid)
				{
					State = DemonState.Restart;
					Status = DemonStatus.Warning;
					return;
				}

				State = DemonState.Handshake;				
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;
			}
		}
		#endregion

		#region Shutdown
		private void ShutdownState()
		{
			Disconnect();
			State = DemonState.None;
			Status = DemonStatus.None;

			if (Owner.Valid)
			{
				try
				{
					// NOTE: No need to close the connection
					QuitRequestCommand command = new QuitRequestCommand(Owner);
					command.Execute();
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}
			}
		}
		#endregion

		#region Handshake
		private void HandshakeState()
		{
			try
			{
				// NOTE: No need to close the connection
				HandshakeRequestCommand command = new HandshakeRequestCommand(Owner);
				HttpCode code = command.Execute();
				if (code != HttpCode.Ok)
				{
					State = DemonState.Restart;
					Status = DemonStatus.Error;
					return;
				}

				State = DemonState.Join;
				Status = DemonStatus.Info;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;
			}
		}
		#endregion

		#region Join
		private void JoinState()
		{
			try
			{
				// NOTE: No need to close the connection
				JoinRequestCommand command = new JoinRequestCommand(Owner);
				command.Execute();

				State = DemonState.Tunnel;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;				
			}
		}
		#endregion

		#region Tunnel
		private void TunnelState()
		{
			try
			{
				Connect();

				// NOTE: No need to close the connection
				TunnelRequestCommand command = new TunnelRequestCommand(Owner, Connection);
				command.Execute();

				State = DemonState.Listen;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);				
				State = DemonState.Restart;
				Status = DemonStatus.Error;
			}
		}
		#endregion

		#region Quit
		private void QuitState()
		{
			try
			{
				// NOTE: No need to close the connection
				QuitRequestCommand command = new QuitRequestCommand(Owner);
				HttpCode code = command.Execute();
				if (code != HttpCode.Ok)
					Status = DemonStatus.Warning;

				Sleep(DemonTimeout.Ping, DemonState.Init);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;
			}
		}
		#endregion

		#region Listen
		private void ListenState()
		{
			NetworkChannel channel = null;

			try
			{
				// Connect
				if (!Connected)
				{
					State = DemonState.Restart; 
					Status = DemonStatus.Info;						
					return;
				}

				// NOTE: No need to close the channel
				channel = new NetworkChannel(Connection);
				Status = DemonStatus.Success;

				// Request	
				HttpRequest httpRequest;
				channel.Receive(out httpRequest);
				if (httpRequest == null)
				{
					Status = DemonStatus.Warning;
					return;
				}

				// TODO: Version check
				//
				// Message
				SessionComponent session = Owner.Get<SessionComponent>();
				string decrypted = session.Decrypt(httpRequest.Data);
				JsonPacket jsonRequest = JsonPacket.Parse(decrypted);
				JsonMessage jsonRequestMessage = JsonMessage.Parse(jsonRequest.Message);
				JsonType jsonType = jsonRequestMessage.Type;

				switch (jsonType)
				{
					case JsonType.Ping:
					{
						PingResponseCommand command = new PingResponseCommand(Owner, Connection);
						command.Execute(httpRequest, jsonRequest);
						break;
					}
					case JsonType.Search:
					{
						JsonAction action = jsonRequestMessage.Action;

						switch (action)
						{
							case JsonAction.Request:
							{
								SearchRequestCommand command = new SearchRequestCommand(Owner, Connection);
								command.Execute(httpRequest, jsonRequest);
								break;
							}
							case JsonAction.Response:
							{
								SearchResponseCommand command = new SearchResponseCommand(Owner, Connection);
								List<FileComponent> list = command.Execute(httpRequest, jsonRequest);
								OnListAdded(list);
								break;
							}
							default:
							{
								channel.SendBadRequest();
								break;
							}
						}

						break;
					}
					case JsonType.Browse:
					{
						BrowseResponseCommand command = new BrowseResponseCommand(Owner, Connection);
						command.Execute(httpRequest, jsonRequest);
						break;
					}
					case JsonType.Download:
					{
						// TODO
						//
						DownloadResponseCommand command = new DownloadResponseCommand(Owner, Connection);// { Handler = UploadEvent };
						command.Execute(httpRequest, jsonRequest);
						break;
					}
					default:
					{
						channel.SendBadRequest();
						State = DemonState.Restart; 
						Status = DemonStatus.Info;							
						return;
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Status = DemonStatus.Error;

				if (channel != null)
				{
					try
					{
						channel.SendInternalServerError();
					}
					catch (Exception) { }
				}
			}
		}
		#endregion
	}
}
