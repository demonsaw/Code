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
	using DemonSaw.Controller;
	using DemonSaw.Entity;
	using DemonSaw.Network;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Net;

	public class XamlUtil
	{
		// Controllers
		private static LogController LogController { get { return Global.LogController; } }
		private static ClientController ClientController { get { return Global.ClientController; } }
		private static FileController FileController { get { return Global.FileController; } }

		//  OLD
		//		
		private static DownloadController DownloadController { get { return Global.DownloadController; } }
		private static UploadController UploadController { get { return Global.UploadController; } }

		#region Local Client
		public bool IsClientSelected() 
		{
			return ClientController.SelectedItem != null; 
		}

		public ObservableCollection<Entity> GetClients() 
		{
			return ClientController.Collection; 
		}

		public Entity GetClient()
		{
			return (ClientController != null) ? ClientController.SelectedItem : null;
		}
		#endregion

		#region Local File
		public bool IsFileSelected()
		{
			//if (FileController == null)
				return false;

			//FileData data = FileController.SelectedItems.Find(i => i.IsFile());
			//return data != null;
		}

		public bool IsSharedFolderSelected()
		{
			if (ClientController == null)
				return false;

			//ClientData client = ClientController.SelectedItem;
			//if (client == null)
			//	return false;

			//List<FileData> folders = FileController.SelectedItems.FindAll(i => i.IsDirectory());
			//foreach (FileData folder in folders)
			//{
			//	if (client.SharedFolders.Contains(folder))
			//		return true;
			//}

			return false;
		}
		#endregion

		#region Download
		public bool HasActiveDownloads()
		{
			if (DownloadController == null)
				return false;

			// TODO
			//
			//foreach (DownloadData data in DownloadController.Items)
			//{
			//	if (data.Inactive)
			//		return true;
			//}

			return false;
		}
		#endregion

		#region Upload
		public bool HasActiveUploads()
		{
			if (UploadController == null)
				return false;

			// TODO
			//
			//foreach (UploadData data in UploadController.Items)
			//{
			//	if (data.Inactive)
			//		return true;
			//}

			return false;
		}
		#endregion

		#region Network
		public List<string> GetAddresses()
		{
			List<string> list = new List<string>();
			foreach (IPAddress ip in NetworkUtil.GetIPAddresses())
				list.Add(ip.ToString());

			return list;
		}
		#endregion
	}
}
