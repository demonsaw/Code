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

	public sealed class ServerController : EntityController
	{
		// Properties - Helper
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
		#endregion

		#region Utility
		public Entity Add()
		{
			Entity entity = new Entity();

			ServerComponent server = new ServerComponent() { Name = Environment.MachineName, Address = "127.0.0.1" };
			entity.Add(server);

			ServerOptionsComponent options = new ServerOptionsComponent();
			entity.Add(options);

			ServerStatusComponent status = new ServerStatusComponent();
			entity.Add(status);

			MachineComponent machine = new ServerMachineComponent();
			entity.Add(machine);

			SessionMapComponent sessions = new SessionMapComponent();
			entity.Add(sessions);
#if DEBUG
			LogComponent log = new LogComponent(LogController);
			entity.Add(log);
#endif
			Add(entity);

			return entity;
		}

		public override void Add(Entity entity)
		{
			base.Add(entity);

			// Events
			entity.EntityShutdown += Update;
		}

		public void Start(Entity entity)
		{
			MachineComponent machine = entity.Get<MachineComponent>();
			machine.Start();
		}

		public void Stop(Entity entity)
		{
			MachineComponent machine = entity.Get<MachineComponent>();
			machine.Stop();
		}

		public void Restart(Entity entity)
		{
			MachineComponent machine = entity.Get<MachineComponent>();
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
