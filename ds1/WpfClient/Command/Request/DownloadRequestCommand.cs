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
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Json;
	using DemonSaw.Json.Data;
	using DemonSaw.Json.Message;
	using DemonSaw.Json.Object;
	using DemonSaw.Network;
	using System.Net.Sockets;

	public sealed class DownloadRequestCommand : ClientCommand
	{
		#region Properties
		public string Id { get; private set; }
		public JsonChunk Chunk { get; private set; }
		public byte[] Data { get; private set; }
		#endregion

		#region Constructors
		public DownloadRequestCommand(Entity entity) : base(entity) { }
		public DownloadRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public DownloadRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion

		#region Interface
		public override void Clear()
		{
			base.Clear();

			Id = null;
			Chunk = null;
			Data = null;
		}
		#endregion

		#region Utility
		public HttpCode Execute(string jsonId, FileComponent file, string clientId = null)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request			
			JsonChunk jsonChunk = new JsonChunk() { Size = file.Size };
			JsonClient jsonClient = (clientId != null) ? new JsonClient(clientId) : null;			
			JsonDownloadRequestMessage jsonRequestMessage = new JsonDownloadRequestMessage(jsonId) { Chunk = jsonChunk, Client = jsonClient };

			JsonFile jsonFile = (JsonFile) file;
			JsonDownloadRequestData jsonRequestData = new JsonDownloadRequestData() { File = jsonFile };
			JsonPacket jsonRequest = new JsonPacket(jsonRequestMessage, Group.Encrypt(jsonRequestData));

			HttpRequest httpRequest = new HttpRequest(Session.Id) { Data = Session.Encrypt(jsonRequest) };
			channel.Send(httpRequest);

			// Response
			HttpResponse httpResponse;
			channel.Receive(out httpResponse);
			Code = httpResponse.Code;

			if (httpResponse.Ok)
			{
				JsonPacket jsonResponse = JsonPacket.Parse(Session.Decrypt(httpResponse.Data));
				JsonDownloadResponseMessage jsonResponseMessage = JsonDownloadResponseMessage.Parse(jsonResponse.Message);
				Chunk = jsonResponseMessage.Chunk;
				Id = jsonResponseMessage.Id;

				JsonDownloadResponseData jsonResponseData = JsonDownloadResponseData.Parse(Group.Decrypt(jsonResponse.Data));
				if (jsonResponseData != null)
					Data = jsonResponseData.Data;
#if DEBUG
				jsonResponse.Data = null;
				Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
			}

			return httpResponse.Code;
		}
		#endregion
	}
}
