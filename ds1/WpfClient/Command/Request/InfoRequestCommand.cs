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
	using DemonSaw.Utility;
	using System;
	using System.Net.Sockets;
	using System.Threading;

	public sealed class InfoRequestCommand : ClientCommand
	{
		// Constructors
		public InfoRequestCommand(Entity entity) : base(entity) { }
		public InfoRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public InfoRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Utility
		public void Execute()
		{
			try
			{
				Clear();

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
			Clear();

			// Connect
			NetworkChannel channel = null;

			try
			{
				channel = new NetworkChannel(Connection);

				// Request
				JsonInfoRequestMessage jsonRequestMessage = new JsonInfoRequestMessage();
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
					JsonInfoResponseMessage jsonResponseMessage = JsonInfoResponseMessage.Parse(jsonResponse.Message);

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
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Machine.Status = DemonStatus.Warning;
			}
			finally
			{
				if (channel != null)
					channel.Shutdown();
			}
		}
		#endregion
	}
}
