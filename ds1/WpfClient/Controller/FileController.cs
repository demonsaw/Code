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
	using System.Collections.Generic;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Threading;

	public sealed class FileController : TreeController<FileComponent>
	{
		// Properties - Helper
		private static ClientController ClientController { get { return Global.ClientController; } }

		// Constructors
		public FileController(TreeList view, TextBlock title) : base(view, title) { }

		#region Interface
		public override void Update()
		{
			base.Update();

			if (Application.Current != null)
			{
				Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(delegate
				{
					if (ClientController.Selected)
					{
						Entity entity = ClientController.SelectedItem;
						FileMapComponent files = entity.Get<FileMapComponent>();
						Title.Text = ((files.Count != 1) ? string.Format("{0:n0} Files", files.Count) : "1 File");
					}
					else
					{
						Title.Text = "0 Files";
					}
				}));
			}
		}
		#endregion

		#region Utility
		public override void Add(FileComponent file)
		{
			Entity entity = file.Owner;
			FolderListComponent folders = entity.Get<FolderListComponent>();
			folders.Add(file);

			View.Clear();
		}

		public override bool Remove(FileComponent file)
		{
			bool removed = false;

			if (file.Drive)
			{
				Entity entity = file.Owner;
				FolderListComponent folders = entity.Get<FolderListComponent>();
				removed = folders.Remove(file);

				if (removed)
					View.Clear();
			}

			return removed;
		}

		public override void Select()
		{
			Select(ClientController.SelectedItem);
		}

		public void Select(Entity entity)
		{
			FileComponent file = null;

			if (entity != null)
				file = entity.Get<FileComponent>();

			Select(file);
		}
		#endregion

		#region IO
		public void Open(List<FileComponent> list)
		{
			List<string> items = new List<string>();

			foreach (FileComponent file in list)
				Open(file, items);

			FileUtil.Execute(items);
		}

		private void Open(FileComponent folder, List<string> paths)
		{
			if (folder.File)
				paths.Add(folder.Path);
			else
			{
				foreach (FileComponent file in folder.Children)
					Open(file, paths);
			}
		}
		#endregion
	}
}
