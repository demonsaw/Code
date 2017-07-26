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
	using DemonSaw.Controller;
	using DemonSaw.IO;
	using System;

#if !DEBUG
	using System.Windows;
#endif

	public static class Global
	{
		// Properties
		public static MainWindow Main { get; private set; }
		public static ConfigFile Config { get; private set; }

		public static LogController LogController { get; private set; }
		public static RouterController RouterController { get; private set; }
		public static ServerController ServerController { get; private set; }

		#region Interface
		public static void Create(MainWindow window)
		{
			Main = window;
			Config = new ConfigFile();

			// Controllers
			LogController = new LogController(Main.LogTextBox, Main.LogTextBlock);
			RouterController = new RouterController(Main.RouterListView, Main.RouterListViewTextBlock);
			ServerController = new ServerController(Main.ServerListView, Main.ServerListViewTextBlock);
#if !DEBUG
			Main.DebugTab.IsEnabled = false;
			Main.DebugTab.Visibility = Visibility.Collapsed;
#endif		
		}

		public static void Init()
		{
			LogController.Init();
			RouterController.Init();
			ServerController.Init();
		}

		public static void Shutdown()
		{
			ServerController.Shutdown();
			RouterController.Shutdown();
			LogController.Shutdown();
		}

		#endregion

		#region Utility
		public static void Load()
		{
			try
			{
				Config.Load();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		public static void Save()
		{
			try
			{
				Config.Save();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}
		#endregion
	}
}
