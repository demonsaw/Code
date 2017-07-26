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
	using DemonSaw.Json;
	using DemonSaw.Json.Object;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.IO;
	using System.Threading;

	public sealed class UploadMachine : DataMachine
	{
		// Properties
		private NetworkConnection Connection { get; set; }

		public string Id { get; set; }		
		private JsonFile File { get; set; }
		private JsonChunk Chunk { get; set; }

		// Properties - Helper
		private new UploadData Data { get { return (UploadData) base.Data; } }
		private ClientData Client { get { return Data.Client; } }
		private RemoteRouterData Router { get { return Data.Client.Router; } }

		// Properties - Static
		public static int Count { get; private set; }

		// Constructors
		public UploadMachine(UploadData upload) : base(upload)
		{
			Chunk = upload.Chunk;
			File = new JsonFile();//upload);			
		}

		// Interface
		public override void Start()
		{
			base.Start();

			++Count;
		}

		protected override void Main()
		{
			base.Main();

			Data.Created = Data.Updated = DateTime.Now;

			do
			{
				// Paused
				if (Paused)
				{
					Data.Status = DemonStatus.Paused;
					Thread.Sleep(DemonTimeout.Sleep);
					Update();
					continue;
				}

				// Idle
				if (Data.Idle)
				{
					State = DemonState.Shutdown;
					Data.Status = DemonStatus.Error;
				}

				NextState();

				switch (State)
				{
					case DemonState.Init:
					{
						Init();
						break;
					}
					case DemonState.Connect:
					{
						Connect();
						break;
					}
					case DemonState.Upload:
					{
						Upload();
						break;
					}
					case DemonState.Restart:
					{
						Disconnect();
						Data.Status = Data.Valid ? DemonStatus.Error : DemonStatus.Warning;
						Sleep(DemonTimeout.Seconds, DemonState.Init);
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

		// Utility
		private void Connect()
		{
			try
			{
				Connection = new NetworkConnection(Router.Address, Router.Port);
				State = DemonState.Upload;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
			}
		}

		private void Disconnect()
		{
			if (Connection != null)
			{
				Connection.Stop();
				Connection = null;
			}

			Id = null;
		}

		// States
		private void Init()
		{
			State = Data.Valid ? DemonState.Connect : DemonState.Restart;
		}

		private void Shutdown()
		{
			if (!Data.Finished && (!Data.Cancelled))
				Data.Status = DemonStatus.Error;

			--Count;
			State = DemonState.None;
			Disconnect();
		}

		private void Upload()
		{
			try
			{
				// Request
				//byte[] data = (Chunk != null) ? FileUtil.Read(Data.Path, Chunk.Size, Chunk.Offset) : null;

				//// Command
				//RouterUploadCommand command = new RouterUploadCommand(Connection, Client);
				//command.SetLogHandlers(RequestHandler, ResponseHandler, CommandHandler);
				//HttpCode code = command.Execute(Id, data, Chunk);

				//// Data
				//Data.RemainingBytes -= (Chunk != null) ? Chunk.Size : 0;
				//if (!Data.Paused && !Data.Cancelled)
				//	Data.Status = DemonStatus.Success;

				//// Shutdown
				//if (code == HttpCode.Ok)
				//	Chunk = command.Chunk;
				//else
				//	State = DemonState.Shutdown;
			}
			catch (IOException ex)
			{
				Console.WriteLine(ex.Message);
				Data.Status = DemonStatus.Warning;
				Thread.Sleep(DemonTimeout.File);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Data.Status = DemonStatus.Error;
				State = DemonState.Shutdown;
			}
		}
	}
}
