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

namespace DemonSaw
{
	using DemonSaw.Component;
	using DemonSaw.Controller;
	using DemonSaw.IO;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using DemonSaw.Wpf;
	using DemonSaw.Wpf.TreeList;
	using Microsoft.Win32;
	using System;
	using System.Collections.Generic;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Input;
	using System.Windows.Media;
	using System.Windows.Threading;

	public partial class MainWindow : Window
	{		
		// Controllers
		private static LogController LogController { get { return Global.LogController; } }
		private static MenuController MenuController { get { return Global.MenuController; } }
		private static ClientController ClientController { get { return Global.ClientController; } }
		private static FileController FileController { get { return Global.FileController; } }
		private static SearchController SearchController { get { return Global.SearchController; } }
		private static BrowseController BrowseController { get { return Global.BrowseController; } }

		// OLD
		//				
		private static DownloadController DownloadController { get { return Global.DownloadController; } }
		private static UploadController UploadController { get { return Global.UploadController; } }		

		#region Window
		public MainWindow()
		{
			InitializeComponent();

			AssemblyInfo info = new AssemblyInfo();
			Title = string.Format("DemonSaw - v{0}", info.FileVersion);

			Global.Init();
		}

		private void Window_Initialized(object sender, EventArgs e)
		{
			Global.Create(this);
			Global.Load();
		}

		private void Window_Loaded(object sender, RoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.Get();
			ClientListView.SelectedItem = entity;
			ClientController.Select(entity);
			MenuController.Select(entity);
		}

		private void Window_Closed(object sender, EventArgs e)
		{
			Exit();
		}

		private void TabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
#if DEBUG
			if (DebugTab != null && DebugTab.IsSelected)
			{
				LogController.Update();
			}
#endif
		}
		#endregion

		#region Utility
		private void Exit()
		{
			Global.Shutdown();
			Global.Save();
		}
		#endregion		

		#region Commands
		#region Menu
		private void DefaultCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			e.CanExecute = true;
		}

		private void ExitCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			Application.Current.Shutdown();
		}

		private void AboutCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			AboutWindow wnd = new AboutWindow();
			wnd.Owner = this;
			wnd.ShowDialog();
		}
		#endregion

		#region Client
		private void ClientDefaultCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (ClientController == null)
				return;

			e.CanExecute = ClientController.SelectedItem != null;
		}

		private void ClientAddCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			ClientAddWindow wnd = new ClientAddWindow();
			wnd.Owner = this;

			bool result = (bool) wnd.ShowDialog();
			if (result)
			{
				if (!NetworkUtil.IsPortValid(wnd.Port.Text))
					return;
				
				// Client
				ClientComponent client = new ClientComponent();
				client.Name = wnd.ClientName.Text;
				client.Download = wnd.DownloadFolder.Text;				
				client.Enabled = true;

				// Router
				ServerComponent server = new ServerComponent();
				server.Address = wnd.Address.Text;
				server.Port = int.Parse(wnd.Port.Text);
				server.Passphrase = wnd.Passphrase.Text;

				// Group
				GroupComponent group = new GroupComponent();
				group.Path = wnd.GroupPath.Text;
				GroupImage.Source = group.Image;				

				ClientController.Add(client, server, group);
				ClientListView.SelectedIndex = ClientController.Count - 1;
				ClientController.Select();

				// Controller
				MenuController.Update();
				BrowseController.Select();
			}
		}

		private void ClientRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.SelectedItem;

			if (entity != null)
			{
				GroupImage.Source = null;
				ClientController.Remove(entity);
				FileController.Clear();
			}
		}

		private void ClientUpdateCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			ClientController.Info();
		}

		private void ClientEditCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.SelectedItem;
			ClientEditWindow wnd = new ClientEditWindow(entity) { Owner = this };

			bool result = (bool) wnd.ShowDialog();
			if (result)
			{
				// Client
				ClientComponent client = entity.Get<ClientComponent>();
				client.Name = wnd.ClientName.Text;
				client.Download = wnd.DownloadPath.Text;

				// Group
				GroupComponent group = entity.Get<GroupComponent>();
				bool groupChanged = (entity == MenuController.Entity) && !group.Path.Equals(wnd.GroupPath.Text);
				group.Path = wnd.GroupPath.Text;
				GroupImage.Source = group.Image;

				// Options
				ClientOptionsComponent options = entity.Get<ClientOptionsComponent>();
				options.MaxDownloads = int.Parse(wnd.MaxDownloads.Text);
				options.MaxUploads = int.Parse(wnd.MaxUploads.Text);

				// Machine
				MachineComponent machine = entity.Get<MachineComponent>();
				machine.Restart();

				// Controller
				MenuController.Update();

				if (groupChanged)
				{
					SearchController.Clear();
					BrowseController.Clear();
				}
			}
		}
		#endregion

		#region Router
		private void RouterEditCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.SelectedItem;
			RouterEditWindow wnd = new RouterEditWindow(entity) { Owner = this };

			bool result = (bool) wnd.ShowDialog();
			if (result)
			{
				if (!NetworkUtil.IsPortValid(wnd.Port.Text))
					return;

				// Router
				ServerComponent server = entity.Get<ServerComponent>();
				bool nameChanged = !server.Name.Equals(wnd.RouterName.Text);
				server.Name = wnd.RouterName.Text;
				server.Address = wnd.Address.Text;
				server.Port = int.Parse(wnd.Port.Text);
				server.Passphrase = wnd.Passphrase.Text;

				// Machine
				MachineComponent machine = entity.Get<MachineComponent>();
				machine.Restart();

				// Controller
				MenuController.Update();

				if (nameChanged)
					BrowseController.Select();
			}
		}
		#endregion

		#region File
		private void FileAddCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.SelectedItem;

			System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog()
			{
				Description = "Select the directory that you want to share.",
			};

			System.Windows.Forms.DialogResult result = dlg.ShowDialog();
			if (result == System.Windows.Forms.DialogResult.OK)
			{
				FileComponent file = new FileComponent(dlg.SelectedPath) { Owner = entity };
				FileController.Add(file);
			}

			FileTreeView.UnselectAll();
		}

		private void FileRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			FileController.Remove();
			ClientController.Select();
		}

		private void FileRemoveCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (FileController == null)
				return;

			List<FileComponent> files = FileController.SelectedItems;

			foreach (FileComponent file in files)
			{
				if (file.Drive)
				{
					e.CanExecute = true;
					return;
				}
			}

			e.Handled = true;
		}

		private void FileOpenCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			if (FileController.SelectedItems.Count == 1)
			{
				FileComponent file = FileController.SelectedItem;

				if (file.File)
				{
					file.Open();
					FileTreeView.UnselectAll();
					return;
				}
			}
			
			FileController.Open(FileController.SelectedItems);
			FileTreeView.UnselectAll();
		}
		#endregion

		#region Search
		private void SearchCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			Search();
		}

		private void SearchCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (SearchController == null)
				return;

			e.CanExecute = (SearchKeyword.Text.Length >= Constant.MinKeywordLength);
		}

		private void SearchDownloadCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			SearchController.Download(SearchController.SelectedItems);
		}

		private void SearchDownloadCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (SearchController == null || DownloadController == null)
				return;

			e.CanExecute = SearchController.SelectedItem != null;
		}

		private void SearchRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			SearchController.Remove(SearchController.SelectedItems);
		}

		private void SearchRemoveCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (SearchController == null)
				return;

			e.CanExecute = SearchController.SelectedItem != null;
		}

		private void SearchClearCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			SearchController.Clear();

			SearchKeyword.Text = "";
			SearchKeyword.Focus();
		}

		private void SearchClearCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (SearchController == null)
				return;

			e.CanExecute = !SearchController.Empty;
		}
		#endregion

		#region Browse
		private void BrowseDownloadCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			BrowseController.Download(BrowseController.SelectedItems);
			BrowseTreeView.UnselectAll();
		}

		private void BrowseDownloadCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (BrowseController == null || DownloadController == null)
				return;

			e.CanExecute = BrowseController.SelectedItem != null;
		}

		private void BrowseClearCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			BrowseController.Clear();
		}
		#endregion

		#region Download
		private void DownloadRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DownloadController.Remove(DownloadController.SelectedItems);
		}

		private void DownloadRemoveCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.Finished || (data.Inactive && !data.Queued))
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void DownloadClearCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DownloadController.Clear();
		}

		private void DownloadClearCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			e.CanExecute = !DownloadController.Empty;
		}

		private void DownloadOpenCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanOpen)
			//		FileUtil.Execute(data.Path);
			//}
		}

		private void DownloadOpenCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanOpen)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void DownloadPauseCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanPause)
			//		data.Pause();
			//}
		}

		private void DownloadPauseCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanPause)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void DownloadResumeCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanResume)
			//		data.Resume();
			//}
		}

		private void DownloadResumeCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanResume)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void DownloadCancelCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanCancel)
			//		data.Cancel();
			//}
		}

		private void DownloadCancelCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanCancel)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void DownloadRestartCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanRestart)
			//		data.Restart();
			//}
		}

		private void DownloadRestartCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (DownloadController == null)
				return;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.SelectedItems)
			//{
			//	if (data.CanRestart)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}
		#endregion

		#region  Upload
		private void UploadDefaultCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (UploadController == null)
				return;

			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.Finished || (data.Inactive && !data.Queued))
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void UploadRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			UploadController.Remove(UploadController.SelectedItems);
		}

		private void UploadClearCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			UploadController.Clear();
		}

		private void UploadClearCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (UploadController == null)
				return;

			e.CanExecute = !UploadController.Empty;
		}

		private void UploadOpenCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.CanOpen)
			//		FileUtil.Execute(data.Path);
			//}
		}

		private void UploadPauseCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.CanPause)
			//		data.Pause();
			//}
		}

		private void UploadPauseCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (UploadController == null)
				return;

			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.CanPause)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void UploadResumeCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.Paused)
			//		data.Resume();
			//}
		}

		private void UploadResumeCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (UploadController == null)
				return;

			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.Paused)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}

		private void UploadCancelCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.CanCancel)
			//		data.Cancel();
			//}
		}

		private void UploadCancelCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (UploadController == null)
				return;

			//foreach (UploadData data in UploadController.SelectedItems)
			//{
			//	if (data.CanCancel)
			//	{
			//		e.CanExecute = true;
			//		return;
			//	}
			//}
		}
		#endregion

		#region Log
		private void LogRefreshCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			LogController.Update();
		}

		private void LogClearCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			LogController.Clear();
		}
		#endregion
		#endregion

		#region Data Grid
		private void DataGrid_Select(DataGridRow row)
		{
			// TODO: Add more tabs
			//
			if (ShareTab != null && ShareTab.IsSelected)
			{
				DemonSaw.Entity.Entity entity = (DemonSaw.Entity.Entity) row.Item;
				ClientController.Select(entity);
			}
		}

		private void DataGrid_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);
			if (row == null)
			{
				DataGrid grid = (DataGrid) sender;
				grid.UnselectAll();
				e.Handled = true;
				return;
			}
		}

		private void DataGrid_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);
			if (row == null)
			{
				e.Handled = true;
				return;
			}

			DataGrid grid = (DataGrid) sender;
			if (grid.SelectedItem != null)
				return;

			Point pt = e.GetPosition((UIElement) sender);
			HitTestResult result = VisualTreeHelper.HitTest(grid, pt);
			if (result != null)
			{
				row = WpfUtil.GetGridRow(result.VisualHit);
				if (row != null)
					row.IsSelected = true;
			}
		}

		private void DataGrid_MouseRightButtonUpSelect(object sender, MouseButtonEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);
			if (row == null)
			{
				e.Handled = true;
				return;
			}

			DataGrid grid = (DataGrid) sender;
			Point pt = e.GetPosition((UIElement) sender);
			HitTestResult result = VisualTreeHelper.HitTest(grid, pt);
			if (result != null)
			{
				row = WpfUtil.GetGridRow(result.VisualHit);
				if (row != null)
				{
					row.IsSelected = true;
					DataGrid_Select(row);
				}
			}
		}
		#endregion

		#region TreeList
		private void TreeList_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			TreeListItem item = WpfUtil.GetTreeListItem(e.OriginalSource);
			if (item == null)
			{
				TreeList list = (TreeList) sender;
				list.UnselectAll();
				e.Handled = true;
			}
		}

		private void TreeList_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
		{
			TreeListItem item = WpfUtil.GetTreeListItem(e.OriginalSource);
			if (item == null)
			{
				e.Handled = true;
				return;
			}

			TreeList list = (TreeList) sender;
			if (list.SelectedItem != null)
				return;

			Point pt = e.GetPosition((UIElement) sender);
			HitTestResult result = VisualTreeHelper.HitTest(list, pt);
			if (result != null)
			{
				item = WpfUtil.GetTreeListItem(result.VisualHit);
				if (item != null)
					item.IsSelected = true;
			}
		}
		#endregion

		#region Client
		private void ClientListView_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);
			if (row == null)
			{
				DataGrid grid = (DataGrid) sender;
				grid.UnselectAll();
				e.Handled = true;
			}

			ClientController.Select();
		}

		private void ClientListView_KeyUp(object sender, KeyEventArgs e)
		{
			if ((e.Key == Key.Down) || (e.Key == Key.Up))
				FileController.Select();			
		}

		private void ClientListView_Enabled_CheckedChanged(object sender, RoutedEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);

			if (row != null)
			{
				DemonSaw.Entity.Entity entity = (DemonSaw.Entity.Entity) row.Item;

				if (entity != null)
				{
					CheckBox control = (CheckBox) sender;
					bool enabled = (bool) control.IsChecked;
					ClientComponent client = entity.Get<ClientComponent>();

					if (client.Enabled != enabled)
						ClientController.Toggle(entity);
				}
			}
		}

		private void ClientListView_GroupImageButton_Click(object sender, RoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.SelectedItem;
			if (entity == null)
				return;

			// Group
			GroupComponent group = entity.Get<GroupComponent>();

			OpenFileDialog dlg = new OpenFileDialog()
			{
				Title = "Browse For Group Image",
				FileName = group.Path,
				Filter = "BMP Files (*.bmp)|*.bmp|JPEG Files (*.jpeg)|*.jpeg|JPG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif|ICO Files (*.ico)|*.ico|PNG Files (*.png)|*.png|All files (*.*)|*.*",
				FilterIndex = 7
			};

			if (dlg.ShowDialog() == true)
			{
				// Group
				bool groupChanged = (entity == MenuController.Entity) && !group.Path.Equals(dlg.FileName);
				group.Path = dlg.FileName;
				GroupImage.Source = group.Image;

				// Machine
				MachineComponent machine = entity.Get<MachineComponent>();
				machine.Restart();

				// Controllers
				if (groupChanged)
				{
					SearchController.Clear();
					BrowseController.Clear();
				}
			}
		}

		private void ClientListView_DownloadPathButton_Click(object sender, RoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = ClientController.SelectedItem;
			if (entity == null)
				return;

			// Client
			ClientComponent client = entity.Get<ClientComponent>();

			System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog()
			{
				Description = "Select the directory that you want to use as the download location.",
				SelectedPath = client.Download
			};

			System.Windows.Forms.DialogResult result = dlg.ShowDialog();
			if (result == System.Windows.Forms.DialogResult.OK)
				client.Download = dlg.SelectedPath;
		}
		#endregion

		#region File
		private void FileTreeView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			TreeListItem item = WpfUtil.GetTreeListItem(e.OriginalSource);
			if (item == null)
				return;

			FileComponent file = FileController.SelectedItem;

			if (file != null)
			{
				if (file.File)
				{
					file.Open(); 					
				}
				else
				{
					TreeNode node = FileTreeView.SelectedNode;
					if (node != null)
						FileTreeView.SetIsExpanded(node, !node.IsExpanded);
				}
			}

			FileTreeView.UnselectAll();
		}
		#endregion

		#region Search
		private void Search()
		{
			string keyword = SearchKeyword.Text.Trim().ToLower();
			if (keyword.Length >= Constant.MinKeywordLength)
			{
				FileFilter filter = (FileFilter) SearchFilter.SelectedIndex;
				SearchController.Search(keyword, filter);

				SearchButton.IsEnabled = false;
				SearchKeyword.SelectAll();
				SearchKeyword.Focus();
			}
		}

		private void SearchTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter)
			{
				Search();
				e.Handled = true;
			}
		}

		private void SearchListView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			FileComponent file = SearchController.SelectedItem;
			if (file == null)
				return;

			DownloadController.Download(file);
		}
		#endregion

		#region Browse
		private void BrowseTreeView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			TreeListItem item = WpfUtil.GetTreeListItem(e.OriginalSource);
			if (item == null)
				return;

			BrowseComponent browse = BrowseController.SelectedItem;

			if (browse != null)
			{
				if (browse.File)
				{
					DownloadController.Download(browse);
				}
				else
				{
					TreeNode node = BrowseTreeView.SelectedNode;

					if (node != null)
					{
						if (Application.Current != null)
						{
							Application.Current.Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(delegate
							{
								BrowseTreeView.SetIsExpanded(node, !node.IsExpanded);
							}));
						}
					}
				}
			}

			BrowseTreeView.UnselectAll();
		}
		#endregion

		#region Download
		private void DownloadListView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			DemonSaw.Entity.Entity entity = DownloadController.SelectedItem;
			if (entity == null)
				return;

			FileComponent file = entity.Get<FileComponent>();
			FileUtil.Execute(file.Path);
		}
		#endregion

		#region Upload
		private void UploadListView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			//UploadData data = UploadController.SelectedItem;
			//if (data == null)
			//	return;

			//FileUtil.Execute(data.Path);
		}
		#endregion
	}
}
