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
	using DemonSaw.Network;
	using System.Net.Sockets;

	public sealed class SearchRequestCommand : RouterCommand
	{
		#region Constructors
		public SearchRequestCommand(Entity entity) : base(entity) { }
		public SearchRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public SearchRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion

		#region Utility
		public string Execute(string jsonData)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request
			JsonSearchRequestMessage jsonRequestMessage = new JsonSearchRequestMessage();
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
				// Data
				return jsonResponse.Data;
			}

			return null;
		}
		#endregion
	}
}
