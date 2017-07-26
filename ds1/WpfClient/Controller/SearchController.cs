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
	using DemonSaw.IO;
	using System;
	using System.Collections;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Threading;

	public sealed class SearchController : GridController<FileComponent>
	{
		#region Properties
		private static MainWindow Main { get { return Global.Main; } }
		private static MenuController MenuController { get { return Global.MenuController; } }
		private static DownloadController DownloadController { get { return Global.DownloadController; } }
		#endregion

		#region Constructors
		public SearchController(DataGrid view, TextBlock title) : base(view, title) { }
		#endregion

		#region Interface
		public override void Update()
		{
			base.Update();

			if (Application.Current != null)
			{
				Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(delegate
				{
					Title.Text = ((List.Count != 1) ? string.Format("{0:n0} Files", List.Count) : "1 File");
				}));
			}
		}

		public override void Clear()
		{
			base.Clear();

			if (MenuController.Entity != null)
			{
				Entity entity = MenuController.Entity;
				SearchListComponent searchList = entity.Get<SearchListComponent>(); ;
				searchList.Id = null;				
			}
			
			Select(MenuController.Entity);
		}
		#endregion

		#region Utility
		public override void Stop()
		{
			base.Stop();

			if (Application.Current != null)
			{
				Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Background, new Action(delegate
				{
					Main.SearchButton.IsEnabled = true;
				}));
			}
		}

		public void Select(Entity entity)
		{
			SearchListComponent search = null;

			if (entity != null)
				search = entity.Get<SearchListComponent>();

			Select(search);
		}

		public void Select(SearchListComponent search)
		{
			List = (search != null) ? search.List : EmptyList;
			View.ItemsSource = Collection;
		}
		#endregion

		#region Commands
		public void Download(IEnumerable items)
		{
			foreach (FileComponent file in items)
				Download(file);
		}

		public void Download(FileComponent file)
		{
			DownloadController.Download(file);
		}

		// TODO: Eventually support multiple searches via a TreeView?
		//
		public void Search(string keyword, FileFilter filter = FileFilter.None)
		{
			if (MenuController.Entity == null)
				return;

			Clear();
			Start();

			try
			{
				// NOTE: No need to close the connection
				SearchRequestCommand command = new SearchRequestCommand(MenuController.Entity);
				command.Execute(keyword, filter);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
		#endregion
	}
}
