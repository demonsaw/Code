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

using DemonSaw.IO;
using DemonSaw.Utility;
using Microsoft.Win32;
using System;
using System.Windows;
using System.Windows.Controls;

namespace DemonSaw
{
	public partial class ClientAddWindow : Window
	{
		public ClientAddWindow()
		{
			InitializeComponent();
		}

		#region Window
		private void ClientAddWindow_Loaded(object sender, RoutedEventArgs e)
		{
#if DEBUG
			Address.Text = Default.Address;
#endif
			Port.Text = Default.Port.ToString();
			ClientName.Text = Environment.MachineName;
			DownloadFolder.Text = FileUtil.GetDownloadFolder();
			SetGroupImage();

			ClientName.Focus();
		}

		private void TextBox_GotFocus(object sender, RoutedEventArgs e)
		{
			TextBox control = (TextBox) sender;
			control.SelectAll();
		}

		private void ClientAddWindow_Okay(object sender, RoutedEventArgs e)
		{
			DialogResult = true;
		}
		#endregion

		#region Utility
		private void SetGroupImage()
		{
			GroupImage.Source = (!string.IsNullOrEmpty(GroupPath.Text)) ? FileUtil.ReadBitmap(GroupPath.Text) : Default.NoImage;
		}
		#endregion

		#region Client
		private void ClientAddWindow_GroupImageOpen(object sender, RoutedEventArgs e)
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
				GroupPath.Focus();
				SetGroupImage();
			}
		}

		private void ClientAddWindow_GroupImageClear(object sender, RoutedEventArgs e)
		{
			GroupPath.Text = "";
			SetGroupImage();
			GroupPath.Focus();
		}

		private void ClientAddWindow_DownloadFolderOpen(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog()
			{
				Description = "Select the directory that you want to use as the download location.",
				SelectedPath = DownloadFolder.Text
			};

			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				DownloadFolder.Text = dlg.SelectedPath;
				DownloadFolder.Focus();
			}				
		}

		private void ClientAddWindow_DownloadFolderClear(object sender, RoutedEventArgs e)
		{
			DownloadFolder.Text = "";
			DownloadFolder.Focus();
		}
		#endregion
	}
}
