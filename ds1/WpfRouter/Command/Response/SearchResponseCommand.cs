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
	using DemonSaw.Network;
	using DemonSaw.Security;
	using System;
	using System.Net.Sockets;
	using System.Threading;

	public sealed class SearchResponseCommand : RouterCommand
	{
		// Classes
		private class CommandState
		{
			public string Id { get; set; }
			public string Data { get; set; }
			public Entity Source { get; set; }
			public Entity Target { get; set; }
		}

		// Constructors
		public SearchResponseCommand(Entity entity) : base(entity) { }
		public SearchResponseCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public SearchResponseCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public HttpCode Execute(string jsonId, string jsonData)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonSearchResponseMessage jsonRequestMessage = new JsonSearchResponseMessage(jsonId);
			JsonPacket jsonRequest = new JsonPacket(jsonRequestMessage, jsonData);
			HttpRequest httpRequest = new HttpRequest() { Data = Session.Encrypt(jsonRequest) };

			// Response
			HttpResponse httpResponse;

			// NOTE: We need to lock this send/receive message pairs
			lock (Socket)
			{
				channel.Send(httpRequest);
				channel.Receive(out httpResponse);
			}

			Code = httpResponse.Code;

			if (httpResponse.Ok)
			{
				JsonPacket jsonResponse = JsonPacket.Parse(Session.Decrypt(httpResponse.Data));
#if DEBUG
				Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
			}

			return Code;
		}

		public void Execute(HttpRequest httpRequest, JsonPacket jsonRequest, SessionComponent session)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonSearchRequestMessage jsonRequestMessage = JsonSearchRequestMessage.Parse(jsonRequest.Message);

			// Data
			Entity entity = session.Owner;
			GroupComponent group = entity.Get<GroupComponent>();
			EntityList entities = GroupList.Get(group.Id);

			if (entities == null)
			{
				channel.SendNotFound();
				return;
			}

			// Response
			string jsonId = SecurityUtil.CreateKeyString();
			JsonSearchResponseMessage jsonResponseMessage = new JsonSearchResponseMessage(jsonId);
			JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage);

			HttpResponse httpResponse = new HttpResponse() { Data = session.Encrypt(jsonResponse) };
			channel.Send(httpResponse);
			
			// Optimization: Keep track of the search list for faster downloads
			SearchListComponent download = entity.Get<SearchListComponent>();
			download.Clear();

			// Command
			lock (entities)
			{
				foreach (Entity e in entities)
				{
					// Do not search yourself
					if (entity.Id.Equals(e.Id))
						continue;

					TunnelComponent tunnel = e.Get<TunnelComponent>();
					if (!tunnel.Connected)
						continue;

					CommandState state = new CommandState()
					{
						Id = jsonId,
						Data = jsonRequest.Data,
						Source = entity,
						Target = e
					};

					Thread thread = new Thread(new ParameterizedThreadStart(ExecuteThread)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
					thread.Start(state);
				}
			}
#if DEBUG
			Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
		}

		private void ExecuteThread(object obj)
		{
			try
			{
				// Data
				CommandState state = (CommandState) obj;

				// Request
				Entity entity = state.Target;
				TunnelComponent tunnel = entity.Get<TunnelComponent>();
				SearchRequestCommand cmdRequest = new SearchRequestCommand(entity, tunnel.Connection);
				string jsonData = cmdRequest.Execute(state.Data);
				if (string.IsNullOrEmpty(jsonData))
					return;

				// Response
				entity = state.Source;
				tunnel = entity.Get<TunnelComponent>();
				SearchResponseCommand cmdResponse = new SearchResponseCommand(entity, tunnel.Connection);
				HttpCode code = cmdResponse.Execute(state.Id, jsonData);
				if (code == HttpCode.Ok)
				{
					SearchListComponent download = entity.Get<SearchListComponent>();
					download.Add(state.Target);
				}			
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
		#endregion
	}
}