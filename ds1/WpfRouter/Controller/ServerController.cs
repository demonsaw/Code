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

namespace DemonSaw.Controller
{
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using System;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Threading;

	public class ServerController : EntityController
	{
		// Properties - Helper
		private static RouterController RouterController { get { return Global.RouterController; } }
		private static LogController LogController { get { return Global.LogController; } }

		// Constructors
		public ServerController(DataGrid view, TextBlock title) : base(view, title) { }

		#region Interface
		public override void Update()
		{
			base.Update();

			if (Application.Current != null)
			{
				Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(delegate
				{
					Title.Text = ((List.Count != 1) ? string.Format("{0:n0} Servers", List.Count) : "1 Server");
				}));
			}
		}

		public override void Clear()
		{
			base.Clear();

			List = EmptyList;
			Update();
		}

		public override void Select(Entity entity)
		{
			if (entity != null)
			{
				ServerListComponent servers = entity.Get<ServerListComponent>();
				List = servers.List;
			}
			else
			{
				List = EmptyList;
			}

			View.ItemsSource = List.Collection;

			Update();
		}
		#endregion

		#region Utility
		public Entity Add()
		{
			Entity entity = new Entity();

			ServerComponent server = new ServerComponent() { Address = "127.0.0.1" };
			entity.Add(server);

			MachineComponent machine = new ServerMachineComponent();
			entity.Add(machine);

			SessionComponent session = new SessionComponent();
			entity.Add(session);
#if DEBUG
			LogComponent log = new LogComponent(LogController);
			entity.Add(log);
#endif
			Add(entity);

			return entity;
		}

		public void Start(Entity entity)
		{
			ServerMachineComponent machine = entity.Get<ServerMachineComponent>();
			machine.Start();
		}

		public void Stop(Entity entity)
		{
			ServerMachineComponent machine = entity.Get<ServerMachineComponent>();
			machine.Stop();
		}

		public void Restart(Entity entity)
		{
			ServerMachineComponent machine = entity.Get<ServerMachineComponent>();
			machine.Restart();
		}

		public void Toggle(Entity entity)
		{
			ServerComponent server = entity.Get<ServerComponent>();
			MachineComponent machine = entity.Get<MachineComponent>();
			server.Enabled = !server.Enabled;

			if (server.Enabled)
				machine.Start();
			else
				machine.Stop();
		}
		#endregion
	}
}
