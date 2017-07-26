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
	using DemonSaw.Command.Request;
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using DemonSaw.Utility;
	using System;
	using System.Threading;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Media.Imaging;
	using System.Windows.Threading;

	public sealed class ClientController : EntityController
	{
		#region Properties
		private Timer PingTimer { get; set; }

		private static MainWindow Main { get { return Global.Main; } }
		private static LogController LogController { get { return Global.LogController; } }
		private static MenuController MenuController { get { return Global.MenuController; } }
		private static FileController FileController { get { return Global.FileController; } }
		private static SearchController SearchController { get { return Global.SearchController; } }
		private static DownloadController DownloadController { get { return Global.DownloadController; } }
		private static UploadController UploadController { get { return Global.UploadController; } }
		#endregion

		#region Constructors
		public ClientController(DataGrid view, TextBlock title) : base(view, title) { }
		#endregion

		#region Interface
		public override void Init()
		{
			base.Init();

			PingTimer = new Timer(delegate { Ping(); }, PingTimer, DemonTimeout.Ping, DemonTimeout.Ping);
		}

		public override void Shutdown()
		{
			base.Shutdown();

			if (PingTimer != null)
			{
				PingTimer.Dispose();
				PingTimer = null;
			}
		}

		public override void Update()
		{
			base.Update();

			if (Application.Current != null)
			{
				Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(delegate
				{
					Title.Text = ((List.Count != 1) ? string.Format("{0:n0} Users", List.Count) : "1 User");
				}));
			}
		}
		#endregion

		#region Utility
		public void Add(ClientComponent client, ServerComponent server, GroupComponent group)
		{
			Entity entity = new Entity();
			entity.Add(client);

			ClientOptionsComponent options = new ClientOptionsComponent();
			entity.Add(options);

			entity.Add(server);

			entity.Add(group);

			FolderListComponent folderList = new FolderListComponent();
			entity.Add(folderList);

			FileMapComponent fileMap = new FileMapComponent();
			entity.Add(fileMap);

			FolderMapComponent folderMap = new FolderMapComponent();
			entity.Add(folderMap);

			ClientMachineComponent machine = new ClientMachineComponent();
			entity.Add<MachineComponent>(machine);

			SessionComponent session = new SessionComponent();
			entity.Add(session);

			FileComponent file = new FileComponent();
			entity.Add(file);

			SearchListComponent search = new SearchListComponent(SearchController.Dispatcher);
			entity.Add(search);

			BrowseComponent browse = new BrowseComponent();
			entity.Add(browse);

			DownloadListComponent download = new DownloadListComponent(DownloadController.Dispatcher);
			entity.Add(download);

			UploadListComponent upload = new UploadListComponent(UploadController.Dispatcher);
			entity.Add(upload);
#if DEBUG
			LogComponent log = new LogComponent(LogController);
			entity.Add(log);
#endif
			Add(entity);
		}

		public void Add()
		{
			Entity entity = new Entity();

			ClientComponent client = new ClientComponent() { Name = Environment.MachineName };
			entity.Add(client);			

			ClientOptionsComponent options = new ClientOptionsComponent();
			entity.Add(options);

			ServerComponent server = new ServerComponent();
			entity.Add(server);

			GroupComponent group = new GroupComponent();
			entity.Add(group);

			FolderListComponent folderList = new FolderListComponent();
			entity.Add(folderList);

			FileMapComponent fileMap = new FileMapComponent();
			entity.Add(fileMap);

			FolderMapComponent folderMap = new FolderMapComponent();
			entity.Add(folderMap);

			ClientMachineComponent machine = new ClientMachineComponent();
			entity.Add<MachineComponent>(machine);

			SessionComponent session = new SessionComponent();
			entity.Add(session);

			FileComponent file = new FileComponent();
			entity.Add(file);

			SearchListComponent search = new SearchListComponent(SearchController.Dispatcher);
			entity.Add(search);

			BrowseComponent browse = new BrowseComponent();
			entity.Add(browse);

			DownloadListComponent download = new DownloadListComponent(DownloadController.Dispatcher);
			entity.Add(download);

			UploadListComponent upload = new UploadListComponent(UploadController.Dispatcher);
			entity.Add(upload);
#if DEBUG
			LogComponent log = new LogComponent(LogController);
			entity.Add(log);
#endif
			Add(entity);
		}

		public override void Add(Entity entity)
		{
			base.Add(entity);

			// Events
			ClientComponent client = entity.Get<ClientComponent>();
			client.ComponentChanged += Update;

			FolderListComponent folders = entity.Get<FolderListComponent>();
			folders.ComponentChanged += FileController.Update;

			ClientMachineComponent machine = (ClientMachineComponent) entity.Get<MachineComponent>();
			machine.ListAdded += SearchController.Add;


			MenuController.Add(entity);
		}

		public override bool Remove(Entity entity)
		{
			MenuController.Remove(entity);

			return base.Remove(entity);
		}

		public void Start(Entity entity)
		{
			ClientMachineComponent machine = entity.Get<ClientMachineComponent>();
			machine.Start();
		}

		public void Stop(Entity entity)
		{
			ClientMachineComponent machine = entity.Get<ClientMachineComponent>();
			machine.Stop();
		}

		public void Restart(Entity entity)
		{
			ClientMachineComponent machine = entity.Get<ClientMachineComponent>();
			machine.Restart();
		}

		public override void Select(Entity entity)
		{
			BitmapImage image = null;

			if (entity != null)
			{
				GroupComponent group = entity.Get<GroupComponent>();
				image = group.Image;

				FileComponent file = entity.Get<FileComponent>();
				FileController.Select(file);
			}
			else
			{
				FileController.Select(entity);
			}

			Main.GroupImage.Source = image;			
		}

		public void Toggle(Entity entity)
		{
			ClientComponent client = entity.Get<ClientComponent>();
			MachineComponent machine = entity.Get<MachineComponent>();
			client.Enabled = !client.Enabled;

			if (client.Enabled)
				machine.Start();
			else
				machine.Stop();
		}
		#endregion

		#region Commands
		public void Info()
		{
			lock (List)
			{
				foreach (Entity entity in List)
				{
					if (entity.Invalid)
						continue;

					try
					{
						// NOTE: No need to close the connection
						InfoRequestCommand command = new InfoRequestCommand(entity);
						command.Execute();
					}
					catch (Exception ex)
					{
						Console.WriteLine(ex.Message);
					}
				}
			}
		}

		private void Ping()
		{
			lock (List)
			{
				foreach (Entity entity in List)
				{
					if (entity.Invalid)
						continue;

					try
					{
						// NOTE: No need to close the connection
						PingRequestCommand command = new PingRequestCommand(entity);
						command.Execute();
					}
					catch (Exception ex)
					{
						Console.WriteLine(ex.Message);
					}
				}
			}			
		}

		#endregion
	}
}
