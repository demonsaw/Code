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
	using DemonSaw.Utility;
	using DemonSaw.Wpf;
	using System;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Input;
	using System.Windows.Media;

	public partial class MainWindow : Window
	{
		// Properties - Helper
		private static LogController LogController { get { return Global.LogController; } }
		private static ServerController Server { get { return Global.ServerController; } }

		public MainWindow()
		{
			InitializeComponent();

			AssemblyInfo info = new AssemblyInfo();
			Title = string.Format("DemonSaw - v{0}", info.FileVersion);

		   Global.Init();
		}

		#region Utility
		private void Exit()
		{
			Global.Save();
			Global.Shutdown();
		}
		#endregion

		#region Window
		private void Window_Initialized(object sender, EventArgs e)
		{
			Global.Create(this);
			Global.Load();
		}

		private void Window_Loaded(object sender, RoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = Server.Get();
			ServerListView.SelectedItem = entity;
			Server.Select(entity);
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
			AboutWindow wnd = new AboutWindow() { Owner = this };
			wnd.ShowDialog();
		}
		#endregion

		#region Server
		private void ServerDefaultCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (Server == null)
				return;

			e.CanExecute = (Server.SelectedItem != null);
		}

		private void ServerAddCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			Server.Add();
		}

		private void ServerRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{			
			Server.Remove(Server.SelectedItem);
		}

		private void ServerUpdateCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			Server.Update();
		}

		private void ServerEditCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = Server.SelectedItem;

			ServerComponent server = entity.Get<ServerComponent>();
			ServerOptionWindow wnd = new ServerOptionWindow() { Title = server.Name, Owner = this };

			ServerOptionsComponent options = entity.Get<ServerOptionsComponent>();
			wnd.MaxTransfers.Text = options.MaxTransfers.ToString();
			wnd.MaxChunks.Text = options.MaxChunks.ToString();
			wnd.ChunkSize.Text = (options.ChunkSize >> 10).ToString();

			//wnd.Ping.IsChecked = options.Ping;
			//wnd.Info.IsChecked = options.Info;

			bool result = (bool) wnd.ShowDialog();
			if (result)
			{
				options.MaxTransfers = int.Parse(wnd.MaxTransfers.Text);
				options.MaxChunks = int.Parse(wnd.MaxChunks.Text);
				options.ChunkSize = int.Parse(wnd.ChunkSize.Text) << 10;

				//options.Ping = (bool) wnd.Ping.IsChecked;
				//options.Info = (bool) wnd.Info.IsChecked;
			}
		}
		#endregion

		#region Log
		private void LogUpdateCommand_Executed(object sender, ExecutedRoutedEventArgs e)
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
			if (ServerTab != null && ServerTab.IsSelected)
			{
				DemonSaw.Entity.Entity entity = (DemonSaw.Entity.Entity) row.Item;
				Server.Select(entity);
			}
		}

		private void DataGrid_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);
			if (row == null)
			{
				DataGrid grid = (DataGrid)sender;
				grid.UnselectAll();
				e.Handled = true;
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

		#region Server
		private void ServerListView_Enabled_CheckedChanged(object sender, RoutedEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);

			if (row != null)
			{
				DemonSaw.Entity.Entity entity = (DemonSaw.Entity.Entity) row.Item;

				if (entity != null)
				{
					CheckBox control = (CheckBox) sender;
					bool enabled = (bool) control.IsChecked;
					ServerComponent server = entity.Get<ServerComponent>();

					if (server.Enabled != enabled)
						Server.Toggle(entity);
				}
			}
		}

		private void ServerListView_Address_DropDownClosed(object sender, EventArgs e)
		{
			DemonSaw.Entity.Entity entity = Server.SelectedItem;
			if (entity == null)
				return;

			ComboBox control = (ComboBox) sender;
			ServerComponent server = entity.Get<ServerComponent>();

			if (!server.Address.Equals(control.SelectedItem))
			{
				server.Address = (string) control.SelectedItem;
				Server.Restart(entity);
			}
		}
		#endregion	
	}
}
