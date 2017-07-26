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
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.IO;
	using DemonSaw.Json;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.Diagnostics;
	using System.IO;
	using System.Threading;

	public sealed class DownloadMachineComponent : MachineComponent, IDisposable
	{
		#region Properties
		private string Id { get; set; }
		private string ClientId { get; set; }
		// public static int Count { get; private set; }

		private NetworkConnection Connection { get; set; }		
		private bool Connected { get { return (Connection != null) ? Connection.Connected : false; } }

		private Entity Entity
		{
			get
			{
				EntityStoreComponent store = Owner.Get<EntityStoreComponent>();
				return store.Entity;
			}
		}
		#endregion

		#region Constructors
		public DownloadMachineComponent() { }
		public DownloadMachineComponent(string clientId) { ClientId = clientId; }
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

			FileComponent file = Owner.Get<FileComponent>();
			ClientComponent client = Entity.Get<ClientComponent>();

			try
			{
				// TODO: Support folders!
				//				
				string folder = "";// client.Root;
				string path = !string.IsNullOrEmpty(folder) ? Path.Combine(client.Download, folder) : client.Download;								
				DirectoryUtil.Create(path);

				file.Path = Path.Combine(path, file.Name);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				file.Path = Path.Combine(client.Download, file.Name);
			}

			ServerComponent server = Entity.Get<ServerComponent>();
			if (server.Enabled && Inactive)
				Start();
		}

		public override void Start()
		{
			base.Start();

			//++Count;
		}

		public override void Stop()
		{
			Unlock();

			//--Count;
		}
		#endregion

		#region Utility
		private void Disconnect()
		{
			if (Connection != null)
			{
				Connection.Stop();
				Connection = null;
			}

			Id = null;
		}
		#endregion

		#region Machine
		protected override void Main()
		{
			do
			{
				if (Paused)
				{
					Status = DemonStatus.Paused;
					Thread.Sleep(DemonTimeout.Sleep);
					Owner.Update();
					continue;
				}

				NextState();

				switch (State)
				{
					case DemonState.Init:
					{
						InitState();
						break;
					}
					case DemonState.Connect:
					{
						ConnectState();
						break;
					}
					case DemonState.Download:
					{
						DownloadState();
						break;
					}
					case DemonState.Restart:
					{
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

				State = DemonState.Connect;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;
			}
		}
		#endregion

		#region Connect
		private void ConnectState()
		{
			try
			{
				Disconnect();

				ServerComponent server = Entity.Get<ServerComponent>();
				Connection = new NetworkConnection(server.Address, server.Port);
				State = DemonState.Download;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
			}
		}
		#endregion

		#region Download
		private void DownloadState()
		{
			try
			{
				// Command
				FileComponent file = Owner.Get<FileComponent>();
				DownloadRequestCommand command = new DownloadRequestCommand(Entity, Connection);
				HttpCode code = command.Execute(Id, file, ClientId);

				if (code == HttpCode.Ok)
				{
					byte[] data = command.Data;

					if (data != null)
					{
						// Data
						JsonChunk jsonChunk = command.Chunk;
						Debug.Assert(data.Length == jsonChunk.Size);

						while (true)
						{
							try
							{
								FileUtil.Write(file.Path, data, jsonChunk.Offset);
								break;
							}
							catch (IOException ex)
							{
								Console.WriteLine(ex.Message);
								Status = DemonStatus.Warning;
								Thread.Sleep(DemonTimeout.File);
							}
						}

						TransferComponent transfer = Owner.Get<TransferComponent>();
						transfer.Size += jsonChunk.Size;
						
						if (transfer.Done)
						{
							State = DemonState.Shutdown;
							Status = DemonStatus.Success;
						}							
					}
					else
					{
						if (Id == null)
						{
							FileUtil.Create(file.Path, file.Size);

							if (code == HttpCode.Ok)
							{
								// Data
								Id = command.Id;
								if (!Paused && !Cancelled)
									Status = DemonStatus.Success;
							}
						}
						else
						{
							// TODO: Implement infinite time-out?
							//
							Sleep(DemonTimeout.Seconds);
						}
					}
				}
				else
				{
					State = DemonState.Shutdown;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);				
				State = DemonState.Shutdown;
				Status = DemonStatus.Error;
			}
		}
		#endregion

		#region Shutdown
		private void ShutdownState()
		{
			TransferComponent transfer = Owner.Get<TransferComponent>();
			if (!transfer.Done && !Cancelled)
				Status = DemonStatus.Error;

			State = DemonState.None;

			Disconnect();
		}
		#endregion
	}
}
