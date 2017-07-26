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
	using DemonSaw.IO;
	using DemonSaw.Utility;
	using Microsoft.Win32;
	using System.Windows;
	using System.Windows.Controls;

	public partial class ClientEditWindow : Window
	{
		// Variables
		private DemonSaw.Entity.Entity _entity;

		public ClientEditWindow(DemonSaw.Entity.Entity entity)
		{
			_entity = entity;

			InitializeComponent();
		}

		#region Window
		private void ClientEditWindow_Loaded(object sender, RoutedEventArgs e)
		{
			// Client
			ClientComponent client = _entity.Get<ClientComponent>();
			ClientName.Text = client.Name;
			DownloadPath.Text = client.Download;
			ClientName.Focus();

			// Group
			GroupComponent group = _entity.Get<GroupComponent>();
			GroupPath.Text = group.Path;
			SetGroup();

			// Options
			ClientOptionsComponent options = _entity.Get<ClientOptionsComponent>();
			MaxDownloads.Text = options.MaxDownloads.ToString();
			MaxUploads.Text = options.MaxUploads.ToString();
		}

		private void TextBox_GotFocus(object sender, RoutedEventArgs e)
		{
			TextBox control = (TextBox) sender;
			control.SelectAll();
		}

		private void ClientEditWindow_Okay(object sender, RoutedEventArgs e)
		{
			DialogResult = true;
		}
		#endregion

		#region Utility
		private void SetGroup()
		{
			GroupImage.Source = (!string.IsNullOrEmpty(GroupPath.Text)) ? FileUtil.ReadBitmap(GroupPath.Text) : Default.NoImage;
		}
		#endregion

		#region Client
		private void ClientEditWindow_GroupImageOpen(object sender, RoutedEventArgs e)
		{
			OpenFileDialog dlg = new OpenFileDialog()
			{
				Title = "Browse For Image",
				FileName = GroupPath.Text,
				Filter = "BMP Files (*.bmp)|*.bmp|JPEG Files (*.jpeg)|*.jpeg|JPG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif|ICO Files (*.ico)|*.ico|PNG Files (*.png)|*.png|All files (*.*)|*.*",
				FilterIndex = 7
			};

			if (dlg.ShowDialog() == true)
			{
				GroupPath.Text = dlg.FileName;
				SetGroup();
			}
		}

		private void ClientEditWindow_GroupImageClear(object sender, RoutedEventArgs e)
		{
			GroupPath.Text = "";
			SetGroup();
			GroupPath.Focus();
		}

		private void ClientEditWindow_DownloadFolderOpen(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog()
			{
				Description = "Select the directory that you want to use as the download location.",
				SelectedPath = DownloadPath.Text
			};

			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				DownloadPath.Text = dlg.SelectedPath;
				DownloadPath.Focus();
			}
		}

		private void ClientEditWindow_DownloadFolderClear(object sender, RoutedEventArgs e)
		{
			DownloadPath.Text = "";
			DownloadPath.Focus();
		}
		#endregion
	}
}
