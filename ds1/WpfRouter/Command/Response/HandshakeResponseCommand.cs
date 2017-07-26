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
	using DemonSaw.Security;
	using System.Net.Sockets;

	public sealed class HandshakeResponseCommand : RouterCommand
	{
		#region Constructors
		public HandshakeResponseCommand(Entity entity) : base(entity) { }
		public HandshakeResponseCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public HandshakeResponseCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion

		#region Utility
		public void Execute(HttpRequest httpRequest)
		{
			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Data
			Entity entity = new Entity(null);
			entity.EntityShutdown += Listener;

			EntityIdleComponent idle = new EntityIdleComponent();
			entity.Add(idle);

			SessionComponent session = new SessionComponent() { Key = Key.Parse(httpRequest.Data) };
			entity.Add(session);

			SessionMap.Add(session.Id, entity);
			Server.Sessions = SessionMap.Count;

			// TODO: Use Diffie–Hellman
			//
			// Response
			HttpResponse httpResponse = new HttpResponse(session.Id) { Data = SecurityUtil.Token };
			channel.Send(httpResponse);
#if DEBUG
			LogComponent log = new LogComponent(Log.Controller);
			entity.Add(log);

			Log.Add(httpRequest, httpResponse);
#endif
		}

		public void Execute(HttpRequest httpRequest, JsonPacket jsonRequest, SessionComponent session)
		{
			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonHandshakeRequestMessage jsonRequestMessage = JsonHandshakeRequestMessage.Parse(jsonRequest.Message);
			JsonKey jsonRequestKey = jsonRequestMessage.Key;

			// Response
			JsonKey jsonResponseKey = jsonRequestKey;
			JsonHandshakeResponseMessage jsonResponseMessage = new JsonHandshakeResponseMessage() { Key = jsonResponseKey };
			JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage);

			HttpResponse httpResponse = new HttpResponse() { Data = session.Encrypt(jsonResponse) };
			channel.Send(httpResponse);

			// Data
			session.Key = Key.Parse(jsonRequestKey.Id);
#if DEBUG
			Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
		}
		#endregion
	}
}
