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
	using DemonSaw.Network;
	using System.Net.Sockets;

	public abstract class RouterCommand : DemonCommand
	{
		#region Properties
		public ServerOptionsComponent Options { get { return Entity.Get<ServerOptionsComponent>(); } }

		public ServerListComponent ServerList { get { return Entity.Get<ServerListComponent>(); } }
		public GroupListMapComponent GroupList { get { return Entity.Get<GroupListMapComponent>(); } }

		public EntityMapComponent EntityMap { get { return Entity.Get<EntityMapComponent>(); } }
		public SessionMapComponent SessionMap { get { return Entity.Get<SessionMapComponent>(); } }
		public ClientMapComponent ClientMap { get { return Entity.Get<ClientMapComponent>(); } }
		public TransferMapComponent TransferMap { get { return Entity.Get<TransferMapComponent>(); } }		
		#endregion

		#region Constructors
		public RouterCommand(Entity entity) : base(entity) { }
		public RouterCommand(Entity entity, Socket socket) : base(entity, socket) { }
		public RouterCommand(Entity entity, NetworkConnection connection) : base(entity, connection) { }
		#endregion
	}
}
