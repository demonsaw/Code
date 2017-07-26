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
	using DemonSaw.Network;
	using DemonSaw.Security;
	using System.Net.Sockets;

	public sealed class HandshakeRequestCommand : ClientCommand
	{
		// Constructors
		public HandshakeRequestCommand(Entity entity) : base(entity) { }
		public HandshakeRequestCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public HandshakeRequestCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }

		#region Interface
		public override void Clear()
		{
			base.Clear();

			Session.Clear();
		}
		#endregion

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
				HttpRequest httpRequest = new HttpRequest() { Data = SecurityUtil.Token };
				channel.Send(httpRequest);

				// Response
				HttpResponse httpResponse;
				channel.Receive(out httpResponse);
				Code = httpResponse.Code;

				if (httpResponse.Ok)
				{
					// Data
					Session.Id = httpResponse.Session;
					Session.Key = Key.Parse(httpResponse.Data);
#if DEBUG
					Log.Add(httpRequest, httpResponse);
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
