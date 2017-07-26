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
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.Net.Sockets;

	public sealed class BrowseRequestCommand : ClientCommand
	{
		// Constructors
		public BrowseRequestCommand(Entity entity) : base(entity) { }
		public BrowseRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public BrowseRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public List<BrowseComponent> Execute(string clientId, string fileId)
		{
			Clear();

			NetworkChannel channel = null;			

			try
			{
				// Connect
				channel = new NetworkChannel(Connection);
				
				// Request
				JsonClient jsonClient = new JsonClient(clientId);				
				JsonBrowseRequestMessage jsonRequestMessage = new JsonBrowseRequestMessage() { Client = jsonClient };

				JsonFile jsonFolder = !string.IsNullOrEmpty(fileId) ? new JsonFile(fileId) : null;
				JsonBrowseRequestData jsonRequestData = new JsonBrowseRequestData() { Folder = jsonFolder };
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
					JsonBrowseResponseMessage jsonResponseMessage = JsonBrowseResponseMessage.Parse(jsonResponse.Message);
					Debug.Assert(jsonResponseMessage != null);

					JsonBrowseResponseData jsonResponseData = JsonBrowseResponseData.Parse(Group.Decrypt(jsonResponse.Data));
					Debug.Assert(jsonResponseData != null);

					// Data
					List<BrowseComponent> list = new List<BrowseComponent>();

					// Folders
					if (jsonResponseData.Folders != null)
					{
						foreach (JsonFile json in jsonResponseData.Folders)
						{
							BrowseComponent comp = new BrowseComponent(json.Id, json.Name) { ClientId = jsonClient.Id, Owner = Entity };
							list.Add(comp);
						}
					}

					// Files
					if (jsonResponseData.Files != null)
					{
						foreach (JsonFile json in jsonResponseData.Files)
						{
							BrowseComponent comp = new BrowseComponent(json.Id, json.Name, json.Size) { ClientId = jsonClient.Id, Owner = Entity };
							list.Add(comp);
						}
					}
#if DEBUG
					Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
					return list;
				}
			}
			finally
			{
				if (channel != null)
					channel.Shutdown();
			}

			return null;
		}
		#endregion
	}
}
