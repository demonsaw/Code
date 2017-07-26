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
	using DemonSaw.IO;
	using DemonSaw.Json;
	using DemonSaw.Json.Data;
	using DemonSaw.Json.Message;
	using DemonSaw.Json.Object;
	using DemonSaw.Network;
	using System;
	using System.Collections.Generic;
	using System.Diagnostics;
	using System.Net.Sockets;
	using System.Threading;

	public sealed class SearchRequestCommand : ClientCommand
	{
		#region Properties
		private String Keyword { get; set; }
		private FileFilter Filter { get; set; }
		#endregion

		#region Constructors
		public SearchRequestCommand(Entity entity) : base(entity) { }
		public SearchRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public SearchRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion

		#region Interface
		public override void Clear()
		{
			base.Clear();

			Keyword = null;
			Filter = FileFilter.None;
		}
		#endregion

		#region Utility
		public void Execute(string keyword, FileFilter filter)
		{
			try
			{
				Clear();
				Keyword = keyword;
				Filter = filter;

				Thread thread = new Thread(new ThreadStart(ExecuteThread)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
				thread.Start();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private void ExecuteThread()
		{
			NetworkChannel channel = null;

			try
			{
				// Connect
				channel = new NetworkChannel(Connection);

				// Request
				JsonSearchRequestMessage jsonRequestMessage = new JsonSearchRequestMessage();

				JsonSearch jsonSearch = new JsonSearch() { Keyword = Keyword, Filter = Filter };
				JsonSearchRequestData jsonRequestData = new JsonSearchRequestData() { Search = jsonSearch };
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
					JsonSearchResponseMessage jsonResponseMessage = JsonSearchResponseMessage.Parse(jsonResponse.Message);
					Debug.Assert(!string.IsNullOrEmpty(jsonResponseMessage.Id));

					// Data
					SearchList.Id = jsonResponseMessage.Id;
#if DEBUG
					Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif		
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
			finally
			{
				if (channel != null)
					channel.Shutdown();
			}
		}

		public void Execute(HttpRequest httpRequest, JsonPacket jsonRequest)
		{
			Clear();

			// Connect
			NetworkChannel channel = new NetworkChannel(Connection);

			// Request			
			JsonSearchRequestMessage jsonRequestMessage = JsonSearchRequestMessage.Parse(jsonRequest.Message);
			JsonSearchRequestData jsonRequestData = JsonSearchRequestData.Parse(Group.Decrypt(jsonRequest.Data));
			JsonSearch jsonSearch = jsonRequestData.Search;

			// Data
			List<FileComponent> compFiles = FileMap.Search(jsonSearch);
			if (compFiles.Count == 0)
			{
				channel.SendNotFound();
				return;
			}

			List<JsonFile> jsonFiles = new List<JsonFile>();
			foreach (FileComponent file in compFiles)
			{
				JsonFile jsonFile = (JsonFile) file;
				jsonFiles.Add(jsonFile);
			}

			// Response
			JsonSearchResponseMessage jsonResponseMessage = new JsonSearchResponseMessage();
			JsonSearchResponseData jsonResponseData = new JsonSearchResponseData() { Files = jsonFiles };
			JsonPacket jsonResponse = new JsonPacket(jsonResponseMessage, Group.Encrypt(jsonResponseData));

			HttpResponse httpResponse = new HttpResponse() { Data = Session.Encrypt(jsonResponse) };
			channel.Send(httpResponse);
#if DEBUG
			Log.Add(httpRequest, httpResponse, jsonRequest, jsonResponse);
#endif
		}
		#endregion
	}
}
