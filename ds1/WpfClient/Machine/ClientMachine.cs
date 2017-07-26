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

namespace DemonSaw.Machine
{
	using DemonSaw.Data;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.Net.Sockets;
	using System.Threading;

	public class ClientMachine : DataMachine
	{
		// Properties
		private NetworkConnection Connection { get; set; }

		// Properties - Helper
		private bool Connected { get { return (Connection != null) ? Connection.Connected : false; } }
		private new ClientData Data { get { return (ClientData) base.Data; } }
		private Socket Socket { get { return Connection.Socket; } }

		// Handlers
		//public event DemonDataHandler<SearchData> SearchEvent;
		//public event DemonDataHandler<UploadData> UploadEvent;

		// Constructors
		public ClientMachine(ClientData data) : base(data) { }

		#region Machine
		protected override void Main()
		{
			base.Main();

			do
			{
				NextState();

				switch (State)
				{
					case DemonState.Init:
					{
						Init();
						break;
					}
					case DemonState.Handshake:
					{
						Handshake();
						break;
					}
					case DemonState.Join:
					{
						Join();
						break;
					}
					case DemonState.Tunnel:
					{
						Tunnel();
						break;
					}
					case DemonState.Listen:
					{
						Listen();
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
						Shutdown();
						break;
					}
				}

				Thread.Sleep(DemonTimeout.None);
			}
			while (State != DemonState.None);
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
		private void Connect()
		{
			Disconnect();
			Connection = new NetworkConnection(Data.Router.Address, Data.Router.Port);
		}

		private void Disconnect()
		{
			if (Connection != null)
			{
				Connection.Stop();
				Connection = null;
			}	
		}
		#endregion

		#region Init
		private void Init()
		{
			if (Data.Valid)
			{
				State = DemonState.Handshake;
			}
			else
			{
				Data.Status = DemonStatus.Warning;
				State = DemonState.Restart;
			}
		}
		#endregion

		#region Shutdown
		private void Shutdown()
		{
			Quit();

			Data.Status = DemonStatus.None;
			State = DemonState.None;
		}
		#endregion

		#region Handshake
		private void Handshake()
		{
			try
			{
				// TODO: Do something here (throw exception?)
				//
				// NOTE: No need to close the connection
				//RouterHandshakeCommand command = new RouterHandshakeCommand(Data);
				//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
				//command.Execute();

				State = DemonState.Join;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Data.Status = DemonStatus.Error;
				State = DemonState.Restart;
			}
		}
		#endregion

		#region Join
		private void Join()
		{
			try
			{
				// NOTE: No need to close the connection
				//RouterJoinCommand command = new RouterJoinCommand(Data);
				//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
				//command.Execute();

				State = DemonState.Tunnel;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Data.Status = DemonStatus.Error;
				State = DemonState.Restart;
			}
		}
		#endregion

		#region Tunnel
		private void Tunnel()
		{
			try
			{
				Connect();

				// NOTE: No need to close the connection
				//RouterTunnelCommand command = new RouterTunnelCommand(Connection, Data);
				//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
				//command.Execute();

				State = DemonState.Listen;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Data.Status = DemonStatus.Error;
				State = DemonState.Restart;
			}
		}
		#endregion

		#region Quit
		private void Quit()
		{
			try
			{
				// NOTE: No need to close the connection
				//RouterQuitCommand command = new RouterQuitCommand(Data);
				//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
				//command.Execute();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
		#endregion

		#region Listen
		private void Listen()
		{
			NetworkChannel channel = null;

			try
			{
				// Connect
				if (!Connected)
				{
					Data.Status = DemonStatus.Info;
					State = DemonState.Restart;
					return;
				}

				// NOTE: No need to close the channel
				channel = new NetworkChannel(Connection);
				Data.Status = DemonStatus.Success;

				// Response
				HttpRequest httpRequest;
				channel.Receive(out httpRequest);
				if (httpRequest == null)
					return;

				string decrypted = Data.Session.Decrypt(httpRequest.Data);
				JsonPacket jsonRequest = JsonPacket.Parse(decrypted);
				if (jsonRequest == null)
					return;

				JsonMessage jsonRequestMessage = JsonMessage.Parse(jsonRequest.Message);

				switch (jsonRequestMessage.Type)
				{
					case JsonType.Ping:
					{
						//ClientPingCommand command = new ClientPingCommand(Connection, Data);
						//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
						//command.Execute(httpRequest, jsonRequest);
						break;
					}
					case JsonType.Search:
					{
						JsonAction action = jsonRequestMessage.Action;

						switch (action)
						{
							case JsonAction.Request:
							{
								//ClientSearchCommand command = new ClientSearchCommand(Connection, Data);
								//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
								//command.Execute(httpRequest, jsonRequest);
								break;
							}
							case JsonAction.Response:
							{
								//ClientSearchCommand2 command = new ClientSearchCommand2(Connection, Data) { SearchHandler = SearchEvent };
								//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
								//command.Execute(httpRequest, jsonRequest);
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
						//ClientBrowseCommand command = new ClientBrowseCommand(Connection, Data);
						//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
						//command.Execute(httpRequest, jsonRequest);
						break;
					}
					case JsonType.Download:
					{
						//ClientDownloadCommand command = new ClientDownloadCommand(Connection, Data) { Handler = UploadEvent };
						//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
						//command.Execute(httpRequest, jsonRequest);
						break;
					}
					default:
					{
						channel.SendBadRequest();
						Data.Status = DemonStatus.Info;
						State = DemonState.Restart;
						return;
					}
				}

				Data.Status = DemonStatus.Success;				
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);				
				Data.Status = DemonStatus.Info;
				State = DemonState.Restart;

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
