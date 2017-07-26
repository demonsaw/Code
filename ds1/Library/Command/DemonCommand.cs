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

namespace DemonSaw.Command
{
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using DemonSaw.Http;
	using DemonSaw.Network;
	using System.Net.Sockets;

	public abstract class DemonCommand : EntityCommand
	{
		#region Event
		public EntityEventHandler Listener { get; set; }
		#endregion

		#region Properties
		public HttpCode Code { get; protected set; }

		public ClientComponent Client { get { return Entity.Get<ClientComponent>(); } }
		public ServerComponent Server { get { return Entity.Get<ServerComponent>(); } }
		public MachineComponent Machine { get { return Entity.Get<MachineComponent>(); } }
		public SessionComponent Session { get { return Entity.Get<SessionComponent>(); } }
		public GroupComponent Group { get { return Entity.Get<GroupComponent>(); } }
#if DEBUG
		public LogComponent Log { get { return Entity.Get<LogComponent>(); } }
#endif
		#endregion

		#region Constructors
		public DemonCommand(Entity entity) : base(entity) { }
		public DemonCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public DemonCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion

		#region Interface
		public override void Init()
		{
			Init(Server.Address, Server.Port);
		}

		public override void Clear()
		{
			base.Clear();

			Code = HttpCode.None;
		}
		#endregion
	}
}
