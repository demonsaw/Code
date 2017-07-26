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
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Json.Object;
	using DemonSaw.Network;
	using System.Collections.Generic;
	using System.Net.Sockets;

	public sealed class GroupResponseCommand : RouterCommand
	{
		// Constructors
		public GroupResponseCommand(Entity entity) : base(entity) { }
		public GroupResponseCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public GroupResponseCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public void Execute(HttpRequest httpRequest, JsonPacket jsonRequest, SessionComponent session)
		{
			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonGroupRequestMessage jsonRequestMessage = JsonGroupRequestMessage.Parse(jsonRequest.Message);

			// Data
			Entity entity = session.Owner;
			GroupComponent group = entity.Get<GroupComponent>();
			EntityList entities = GroupList.Get(group.Id);

			if (entities == null)
			{
				channel.SendNotFound();
				return;
			}
			
			List<JsonClient> jsonClients = new List<JsonClient>();

			lock (entities)
			{
				foreach (Entity e in entities)
				{
#if !DEBUG
				if (entity.Id.Equals(e.Id))
					continue;
#endif
					ClientComponent c = e.Get<ClientComponent>();
					JsonClient jsonClient = (JsonClient) c;
					jsonClients.Add(jsonClient);
				}
			}

			if (jsonClients.Count == 0)
			{
				channel.SendNotFound();
				return;
			}

			jsonClients.Sort();

			// Response
			JsonGroupResponseMessage jsonResponseMessage = new JsonGroupResponseMessage() { Clients = jsonClients };
			JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage);

			HttpResponse httpResponse = new HttpResponse() { Data = session.Encrypt(jsonResponse) };
			channel.Send(httpResponse);
#if DEBUG
			Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
		}
		#endregion
	}
}
