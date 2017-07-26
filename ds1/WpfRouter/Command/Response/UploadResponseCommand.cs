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
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System.Net.Sockets;

	public sealed class UploadResponseCommand : RouterCommand
	{
		// TODO: Add callback?
		//public DemonEventDataHandler<UploadData> Handler;

		#region Constructors
		public UploadResponseCommand(Entity entity) : base(entity) { }
		public UploadResponseCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public UploadResponseCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion

		#region Utility
		public JsonAction Execute(HttpRequest httpRequest, JsonPacket jsonRequest, SessionComponent session)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request			
			JsonUploadRequestMessage jsonRequestMessage = JsonUploadRequestMessage.Parse(jsonRequest.Message);

			// Data
			string jsonId = jsonRequestMessage.Id;			
			Entity entity = TransferMap.Get(jsonId);
			if (entity == null)
			{
				channel.SendNotFound();
				return JsonAction.None;
			}

			JsonChunk jsonChunk = jsonRequestMessage.Chunk;
			ChunkComponent transfer = entity.Get<ChunkComponent>();			
			if (jsonChunk != null)
			{
				JsonTransfer jsonTransfer = new JsonTransfer() { Chunk = jsonChunk, Data = jsonRequest.Data };
				transfer.PushData(jsonTransfer);
			}

			if (!transfer.HasChunk())
			{
				channel.SendNotFound();
				return JsonAction.None;
			}

			int delay = 0;
			jsonChunk = null;
			if (transfer.HasDataCapacity())
				jsonChunk = transfer.PopChunk();
			else
				delay = (int) DemonTimeout.File.TotalMilliseconds;

			// Response
			JsonUploadResponseMessage jsonResponseMessage = new JsonUploadResponseMessage(jsonId) { Chunk = jsonChunk, Delay = delay };
			JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage);

			HttpResponse httpResponse = new HttpResponse() { Data = session.Encrypt(jsonResponse) };
			channel.Send(httpResponse);
#if DEBUG
			jsonRequest.Data = null;
			Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
			return JsonAction.Request;
		}
		#endregion
	}
}
