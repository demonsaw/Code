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
	using DemonSaw.IO;
	using DemonSaw.Wpf.TreeList;
	using System;
	using System.Collections;
	using System.IO;
	using System.Threading;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Threading;

	public sealed class BrowseController : TreeController<BrowseComponent>
	{
		#region Properties
		private static MenuController MenuController { get { return Global.MenuController; } }
		private static ClientController ClientController { get { return Global.ClientController; } }
		private static DownloadController DownloadController { get { return Global.DownloadController; } }
		#endregion

		#region Constructors
		public BrowseController(TreeList view, TextBlock title) : base(view, title) { }
		#endregion

		#region Interface
		public override void Init()
		{
			base.Init();

			Clear();
		}

		public override void Update()
		{
			base.Update();

			if (Application.Current != null)
			{
				Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(delegate
				{
					int clients = 0;
					BrowseComponent browse = (BrowseComponent) View.Model;
					if (browse != null)
						clients = browse.Clients;

					Title.Text = ((clients != 1) ? string.Format("{0:n0} Users", clients) : "1 User");
				}));
			}
		}

		public override void Clear()
		{
			BrowseComponent browse = null;

			if (MenuController.Entity != null)
			{
				Entity entity = MenuController.Entity;
				browse = entity.Get<BrowseComponent>();
				browse.Node = null;
			}

			Select(browse);

			base.Clear();
		}
		#endregion

		#region Utility
		public override void Select()
		{
			Select(MenuController.Entity);
		}

		public override void Select(BrowseComponent browse)
		{
			if (browse != null)
			{
				Entity entity = MenuController.Entity;
				ServerComponent server = entity.Get<ServerComponent>();
				string path = server.Name;

				if (string.IsNullOrEmpty(path))
				{
					GroupComponent group = entity.Get<GroupComponent>();
					path = group.Name;
				}

				browse = entity.Get<BrowseComponent>();
				browse.ComponentChanged += Update;
				browse.Path = path;
			}

			base.Select(browse);
		}

		public void Select(Entity entity)
		{
			BrowseComponent browse = null;

			if (entity != null)
				browse = entity.Get<BrowseComponent>();

			Select(browse);
		}
		#endregion

		#region Commands
		public void Download(IEnumerable items)
		{
			Thread thread = new Thread(new ThreadStart(delegate { Download(items, ""); })) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
			thread.Start();
		}

		private void Download(IEnumerable items, string folder)
		{
			foreach (BrowseComponent browse in items)
			{
				if (browse.File)
				{
					Download(browse);
				}
				else if (browse.Folder)
				{
					ClientComponent client = MenuController.SelectedItem;
					string subfolder = Path.Combine(folder, browse.Name);
					string path = Path.Combine(client.Download, subfolder);
					if (!DirectoryUtil.Exists(path))
						DirectoryUtil.Create(path);

					IEnumerable children = browse.GetChildren(browse);
					if (children != null && !folder.Equals(subfolder))
						Download(children, subfolder);
				}
			}
		}

		public void Download(BrowseComponent browse)
		{
			DownloadController.Download(browse);
		}
		#endregion
	}
}
