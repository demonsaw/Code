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
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Network;
	using System.Net.Sockets;

	public sealed class PingResponseCommand : ClientCommand
	{
		// Constructors
		public PingResponseCommand(Entity entity) : base(entity) { }
		public PingResponseCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public PingResponseCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public void Execute(HttpRequest httpRequest, JsonPacket jsonRequest)
		{
			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonPingRequestMessage jsonRequestMessage = JsonPingRequestMessage.Parse(jsonRequest.Message);

			// Response
			JsonPingResponseMessage jsonResponseMessage = new JsonPingResponseMessage();
			JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage);

			HttpResponse httpResponse = new HttpResponse() { Data = Session.Encrypt(jsonResponse) };
			channel.Send(httpResponse);
#if DEBUG
			Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
		}
		#endregion
	}
}
