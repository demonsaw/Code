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

namespace DemonSaw.Command.Response
{
	using DemonSaw.Command.Request;
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Json.Object;
	using DemonSaw.Network;
	using DemonSaw.Security;
	using System;
	using System.Net.Sockets;
	using System.Threading;

	public sealed class DownloadResponseCommand : RouterCommand
	{
		// Classes
		private class CommandState
		{
			public string Id { get; set; }
			public string Data { get; set; }
			public JsonChunk Chunk { get; set; }
			public Entity Entity { get; set; }
		}

		// Constructors
		public DownloadResponseCommand(Entity entity) : base(entity) { }
		public DownloadResponseCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public DownloadResponseCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public JsonAction Execute(HttpRequest httpRequest, JsonPacket jsonRequest, SessionComponent session)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonAction jsonAction = JsonAction.None;
			JsonDownloadRequestMessage jsonRequestMessage = JsonDownloadRequestMessage.Parse(jsonRequest.Message);
			string jsonId = jsonRequestMessage.Id;

			if (jsonId != null)
			{
				// Data
				Entity entity = TransferMap.Get(jsonId);
				if (entity == null)
				{
					channel.SendNotFound();
					return jsonAction;
				}

				ChunkComponent transfer = entity.Get<ChunkComponent>();
				if (transfer == null)
				{
					channel.SendNotFound();
					return jsonAction;
				}
				
				JsonTransfer jsonTransfer = transfer.PopData();				
				if (!transfer.Finished)
				{
					jsonAction = JsonAction.Request;
					entity.Update();
				}
				else
				{
					TransferMap.Remove(jsonId);
				}

				string jsonData = null;
				JsonChunk jsonChunk = null;
				if (jsonTransfer != null)
				{
					jsonData = jsonTransfer.Data; 
					jsonChunk = jsonTransfer.Chunk;
				}

				// Response
				JsonDownloadResponseMessage jsonResponseMessage = new JsonDownloadResponseMessage(jsonId) { Chunk = jsonChunk };
				JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage, jsonData);

				HttpResponse httpResponse = new HttpResponse() { Data = session.Encrypt(jsonResponse) };
				channel.Send(httpResponse);
#if DEBUG
				jsonResponse.Data = null;
				Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif				
			}
			else
			{
				// Data
				Entity entity = session.Owner;
				SearchListComponent search = entity.Get<SearchListComponent>();
				JsonClient jsonClient = jsonRequestMessage.Client;

				// Request
				if (jsonClient == null && search.Empty)
				{
					channel.SendNotFound();
					return jsonAction;
				}

				// Response				
				jsonId = SecurityUtil.CreateKeyString();
				JsonDownloadResponseMessage jsonResponseMessage = new JsonDownloadResponseMessage(jsonId);
				JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage);

				HttpResponse httpResponse = new HttpResponse() { Data = session.Encrypt(jsonResponse) };
				channel.Send(httpResponse);
#if DEBUG
				Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
				// Data
				entity = new Entity(jsonId);
				EntityIdleComponent idle = new EntityIdleComponent();
				entity.Add(idle);

				JsonChunk jsonChunk = jsonRequestMessage.Chunk;
				ChunkComponent transfer = new ChunkComponent(jsonChunk.Size, Options.ChunkSize, Options.MaxChunks);
				entity.Add(transfer);

				TransferMap.Add(entity);

				// Command
				string jsonData = jsonRequest.Data;
				jsonChunk = transfer.PopChunk();

				if (jsonClient == null)
				{
					foreach (Entity e in search)
					{
						CommandState state = new CommandState() { Id = jsonId, Data = jsonData, Chunk = jsonChunk, Entity = e };
						Thread thread = new Thread(new ParameterizedThreadStart(ExecuteThread)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
						thread.Start(state);
					}
				}
				else
				{
					Entity e = ClientMap.Get(jsonClient.Id);
					if (e == null)
					{
						channel.SendNotFound();
						return jsonAction;
					}

					CommandState state = new CommandState() { Id = jsonId, Data = jsonData, Chunk = jsonChunk, Entity = e };
					Thread thread = new Thread(new ParameterizedThreadStart(ExecuteThread)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
					thread.Start(state);
				}

				jsonAction = JsonAction.Request;
			}

			return jsonAction;
		}

		private void ExecuteThread(object obj)
		{
			try
			{
				CommandState state = (CommandState) obj;

				Entity entity = state.Entity;
				TunnelComponent tunnel = entity.Get<TunnelComponent>();
				DownloadRequestCommand command = new DownloadRequestCommand(entity, tunnel.Connection);
				command.Execute(state.Id, state.Data, state.Chunk);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
		#endregion
	}
}
