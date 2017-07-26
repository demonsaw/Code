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

namespace DemonSaw.Wpf
{
	using System.Windows.Input;

	public static class WpfCommand
	{
		#region Menu
		public static readonly RoutedUICommand Exit = new RoutedUICommand
		(
			"Exit",
			"Exit",
			typeof(WpfCommand),
			new InputGestureCollection() { new KeyGesture(Key.F4, ModifierKeys.Alt) }
		);

		public static readonly RoutedUICommand Options = new RoutedUICommand
		(
			"Options",
			"Options",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand About = new RoutedUICommand
		(
			"About",
			"About",
			typeof(WpfCommand)
		);
		#endregion

		#region Client
		public static readonly RoutedUICommand ClientAdd = new RoutedUICommand
		(
			"Client Add",
			"ClientAdd",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand ClientRemove = new RoutedUICommand
		(
			"Client Remove",
			"ClientRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand ClientEdit = new RoutedUICommand
		(
			"Client Edit",
			"ClientEdit",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand ClientUpdate = new RoutedUICommand
		(
			"Client Update",
			"ClientUpdate",
			typeof(WpfCommand)
		);
		#endregion

		#region Router
		public static readonly RoutedUICommand RouterEdit = new RoutedUICommand
		(
			"Router Edit",
			"RouterEdit",
			typeof(WpfCommand)
		);
		#endregion

		#region File
		public static readonly RoutedUICommand FileAdd = new RoutedUICommand
		(
			"File Add",
			"FileAdd",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand FileRemove = new RoutedUICommand
		(
			"File Remove",
			"FileRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand FileOpen = new RoutedUICommand
		(
			"File Open",
			"FileOpen",
			typeof(WpfCommand)
		);
		#endregion

		#region Search
		public static readonly RoutedUICommand Search = new RoutedUICommand
		(
			"Search",
			"Search",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand SearchDownload = new RoutedUICommand
		(
			"Search Download",
			"SearchDownload",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand SearchRemove = new RoutedUICommand
		(
			"Search Remove",
			"SearchRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand SearchClear = new RoutedUICommand
		(
			"Search Clear",
			"SearchClear",
			typeof(WpfCommand)
		);
		#endregion

		#region Browse
		public static readonly RoutedUICommand BrowseDownload = new RoutedUICommand
		(
			"Browse Download",
			"BrowseDownload",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand BrowseClear = new RoutedUICommand
		(
			"Browse Clear",
			"BrowseClear",
			typeof(WpfCommand)
		);
		#endregion

		#region Download
		public static readonly RoutedUICommand DownloadRemove = new RoutedUICommand
		(
			"Download Remove",
			"DownloadRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand DownloadClear = new RoutedUICommand
		(
			"Download Clear",
			"DownloadClear",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand DownloadOpen = new RoutedUICommand
		(
			"Download Open",
			"DownloadOpen",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand DownloadPause = new RoutedUICommand
		(
			"Download Pause",
			"DownloadPause",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand DownloadResume = new RoutedUICommand
		(
			"Download Resume",
			"DownloadResume",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand DownloadCancel = new RoutedUICommand
		(
			"Download Cancel",
			"DownloadCancel",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand DownloadRestart = new RoutedUICommand
		(
			"Download Restart",
			"DownloadRestart",
			typeof(WpfCommand)
		);
		#endregion

		#region Upload
		public static readonly RoutedUICommand UploadRemove = new RoutedUICommand
		(
			"Upload Remove",
			"UploadRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand UploadClear = new RoutedUICommand
		(
			"Upload Clear",
			"UploadClear",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand UploadOpen = new RoutedUICommand
		(
			"Upload Open",
			"UploadOpen",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand UploadPause = new RoutedUICommand
		(
			"Upload Pause",
			"UploadPause",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand UploadResume = new RoutedUICommand
		(
			"Upload Resume",
			"UploadResume",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand UploadCancel = new RoutedUICommand
		(
			"Upload Cancel",
			"UploadCancel",
			typeof(WpfCommand)
		);
		#endregion

		#region Log
		public static readonly RoutedUICommand LogUpdate = new RoutedUICommand
		(
			"Log Update",
			"LogUpdate",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand LogClear = new RoutedUICommand
		(
			"Log Clear",
			"LogClear",
			typeof(WpfCommand)
		);
		#endregion
	};
}
