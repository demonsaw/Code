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
		// Controllers
		private static LogController LogController { get { return Global.LogController; } }
		private static RouterController RouterController { get { return Global.RouterController; } }
		private static ServerController ServerController { get { return Global.ServerController; } }

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
			DemonSaw.Entity.Entity entity = RouterController.Get();
			RouterListView.SelectedItem = entity;
			RouterController.Select(entity);
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

		#region Router
		private void RouterDefaultCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (RouterController == null)
				return;

			e.CanExecute = (RouterController.SelectedItem != null);
		}
		
		private void RouterAddCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			RouterController.Add();
		}

		private void RouterRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			RouterController.Remove(RouterController.SelectedItem);
			ServerController.Clear();
		}	

		private void RouterEditCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			DemonSaw.Entity.Entity entity = RouterController.SelectedItem;

			ServerComponent server = entity.Get<ServerComponent>();
			RouterOptionWindow wnd = new RouterOptionWindow() { Title = server.Name, Owner = this };

			ServerOptionsComponent options = entity.Get<ServerOptionsComponent>();
			wnd.MaxTransfers.Text = options.MaxTransfers.ToString();
			wnd.MaxChunks.Text = options.MaxChunks.ToString();
			wnd.ChunkSize.Text = (options.ChunkSize >> 10).ToString();

			//wnd.Ping.IsChecked = options.Ping; 
			//wnd.Info.IsChecked = options.Info;
			//wnd.Search.IsChecked = options.Search;
			//wnd.Group.IsChecked = options.Group;
			//wnd.Browse.IsChecked = options.Browse;

			bool result = (bool) wnd.ShowDialog();
			if (result)
			{
				options.MaxTransfers = int.Parse(wnd.MaxTransfers.Text);
				options.MaxChunks = int.Parse(wnd.MaxChunks.Text);
				options.ChunkSize = int.Parse(wnd.ChunkSize.Text) << 10;

				//options.Ping = (bool) wnd.Ping.IsChecked; 
				//options.Info = (bool) wnd.Info.IsChecked;
				//options.Search = (bool) wnd.Search.IsChecked;
				//options.Group = (bool) wnd.Group.IsChecked;
				//options.Browse = (bool) wnd.Browse.IsChecked;
			}
		}

		private void RouterUpdateCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			RouterController.Update();
		}
		#endregion

		#region Server
		private void ServerDefaultCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (ServerController == null)
				return;

			e.CanExecute = (ServerController.SelectedItem != null);
		}

		private void ServerAddCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			ServerController.Add();		
		}

		private void ServerRemoveCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			ServerController.Remove(ServerController.SelectedItem);
		}

		private void ServerUpdateCommand_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			ServerController.Update();
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

		private void LogClearCommand_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			if (LogController == null)
				return;

			e.CanExecute = !LogController.Empty;
		}
		#endregion
		#endregion

		#region Data Grid
		private void DataGrid_Select(DataGridRow row)
		{
			// TODO: Add more tabs
			//
			if (RouterTab != null && RouterTab.IsSelected)
			{
				DemonSaw.Entity.Entity entity = (DemonSaw.Entity.Entity) row.Item;
				RouterController.Select(entity);
				ServerController.Select(entity);
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

		#region Router
		private void RouterListView_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			DataGridRow row = WpfUtil.GetGridRow(e.OriginalSource);
			if (row == null)
			{
				DataGrid grid = (DataGrid) sender;
				grid.UnselectAll();
				e.Handled = true;
			}

			RouterController.Select();
		}

		private void RouterListView_KeyUp(object sender, KeyEventArgs e)
		{
			if ((e.Key == Key.Down) || (e.Key == Key.Up))
			{
				RouterController.Select();
			}
		}

		private void RouterListView_Enabled_CheckedChanged(object sender, RoutedEventArgs e)
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
						RouterController.Toggle(entity);
				}
			}
		}

		private void RouterListView_Address_DropDownClosed(object sender, EventArgs e)
		{
			DemonSaw.Entity.Entity entity = RouterController.SelectedItem;
			if (entity == null)
				return;

			ComboBox control = (ComboBox) sender;
			ServerComponent server = entity.Get<ServerComponent>();

			if (!server.Address.Equals(control.SelectedItem))
			{
				server.Address = (string) control.SelectedItem;
				RouterController.Restart(entity);
			}	
		}

		//private void RouterListView_Port_TextChanged(object sender, TextChangedEventArgs e)
		//{
		//	RouterData data = RouterController.SelectedItem;
		//	if (data == null)
		//		return;

		//	TextBox control = (TextBox) sender;
		//	if (NetworkUtil.IsPortValid(control.Text))
		//	{
		//		e.Handled = true;
		//		data.Port = int.Parse(control.Text);
		//		data.Restart();
		//	}
		//}
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
						ServerController.Toggle(entity);
				}
			}
		}
		#endregion

		#region Debug
		private void DebugTab_DestroySessions(object sender, RoutedEventArgs e)
		{
			//DemonSaw.Entity.Entity entity = RouterController.SelectedItem;
			//if (entity == null)
			//	return;

			//ClientMapComponent clients = entity.Get<ClientMapComponent>();
			//foreach (DemonSaw.Entity.Entity ent in clients.Map.Values)
			//{
			//	TunnelComponent tunnel = ent.Get<TunnelComponent>();
			//	PingRequestCommand req = new PingRequestCommand(ent, tunnel.Connection);
			//	HttpCode code = req.Execute();
			//}

			foreach (DemonSaw.Entity.Entity entity in RouterController.List)
			{
				SessionMapComponent sessions = entity.Get<SessionMapComponent>();
				sessions.Clear();
			}
		}
		#endregion
	}
}
