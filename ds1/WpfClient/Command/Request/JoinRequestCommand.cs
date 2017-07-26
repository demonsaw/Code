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

namespace DemonSaw.Command.Request
{
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Json.Object;
	using DemonSaw.Network;
	using System.Net.Sockets;

	public sealed class JoinRequestCommand : ClientCommand
	{
		// Constructors
		public JoinRequestCommand(Entity entity) : base(entity) { }
		public JoinRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public JoinRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public HttpCode Execute()
		{
			Clear();

			// Connect
			NetworkChannel channel = null;

			try
			{
				channel = new NetworkChannel(Connection);

				// Request
				JsonEntity jsonEntity = (JsonEntity) Entity;
				JsonClient jsonClient = (JsonClient) Client;
				JsonGroup jsonGroup = (JsonGroup) Group;

				JsonJoinRequestMessage jsonRequestMessage = new JsonJoinRequestMessage() { Entity = jsonEntity, Client = jsonClient, Group = jsonGroup };
				JsonPacket jsonRequest = new JsonPacket(jsonRequestMessage);

				HttpRequest httpRequest = new HttpRequest(Session.Id) { Data = Session.Encrypt(jsonRequest) };
				channel.Send(httpRequest);

				// Response
				HttpResponse httpResponse;
				channel.Receive(out httpResponse);
				Code = httpResponse.Code;

				if (httpResponse.Ok)
				{					
					JsonPacket jsonResponse = JsonPacket.Parse(Session.Decrypt(httpResponse.Data));
					JsonJoinResponseMessage jsonResponseMessage = JsonJoinResponseMessage.Parse(jsonResponse.Message);

					// Data
					Server.Entities = jsonResponseMessage.Entities; 
					Server.Sessions = jsonResponseMessage.Sessions;
					Server.Clients = jsonResponseMessage.Clients;
					Server.Groups = jsonResponseMessage.Groups;
#if DEBUG
					Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
				}
			}
			finally
			{
				if (channel != null)
					channel.Shutdown();
			}

			return Code;
		}
		#endregion
	}
}
