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

namespace DemonSaw.Data
{
	using DemonSaw.IO;
	using DemonSaw.Utility;
	using DemonSaw.Wpf.TreeList;
	using DemonSaw.Xml;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Threading;
	using System.Windows.Media.Imaging;
	using System.Xml;

	public class ClientData : DemonClientData, ITreeModel, XmlParseable
	{
		// Variables
		public TreeNode Node { get; set; }
		private string _downloadFolder;
		private int _clientCount;
		private int _groupCount;
		private bool _selected;

		// Properties
		public override string Name
		{
			get { return _name; }
			set
			{
				base.Name = value;
				OnName();
			}
		}

		public override bool Enabled
		{
			get { return base.Enabled; }
			set
			{
				base.Enabled = value;

				if (!_enabled)
				{
					ClientCount = Default.ClientCount;
					GroupCount = Default.GroupCount;
				}				
			}
		}

		public List<FileData> SharedFolders { get; set; }
		public Dictionary<string, FileData> FolderMap { get; private set; }
		public Dictionary<string, FileData> FileMap { get; private set; }

		// Group
		public GroupData Group { get; set; }
		public string GroupId { get { return Group.Id; } }
		public string GroupName { get { return Group.Name; } }
		public BitmapImage GroupImage { get { return Group.Image; } }

		public string GroupPath
		{
			get { return Group.Path; }
			set
			{
				Group.Path = value;
				OnPropertyChanged("GroupPath");
				OnPropertyChanged("GroupName");
			}
		}

		// Properties - Helper				
		public RemoteRouterData Router { get; set; }		

		public string DownloadFolder
		{
			get { return _downloadFolder; }
			set
			{
				_downloadFolder = value;
				OnPropertyChanged("DownloadFolder");
			}
		}

		public bool Selected
		{
			get { return _selected; }
			set
			{
				_selected = value;
				OnPropertyChanged("Selected");
			}
		}

		public override bool Valid { get { return !string.IsNullOrEmpty(Name) && Router.Valid; } }

		public int ClientCount
		{
			get { return _clientCount; }
			set
			{
				_clientCount = value;
				OnPropertyChanged("ClientCount");
			}
		}

		public int GroupCount
		{
			get { return _groupCount; }
			set
			{
				_groupCount = value;
				OnPropertyChanged("GroupCount");
			}
		}

		// Handlers
		public event DemonEventDataHandler<ClientData> NameEvent;
		public event DemonEventHandler RestartEvent;
		public event DemonEventHandler FileEvent;

		// Events
		protected void OnName()
		{
			if (NameEvent != null)
				NameEvent(this);
		}

		protected void OnRestart()
		{
			if (RestartEvent != null)
				RestartEvent();
		}

		protected void OnFile()
		{
			if (FileEvent != null)
				FileEvent();
		}
		
		// Constructors
		public ClientData()
		{
			Name = Environment.MachineName;

			Group = new GroupData();
			Router = new RemoteRouterData();
			DownloadFolder = FileUtil.GetDownloadFolder();
			SharedFolders = new List<FileData>();
			FileMap = new Dictionary<string, FileData>();
			FolderMap = new Dictionary<string, FileData>();			
		}

		public ClientData(XmlReader reader) : this() { Read(reader); }

		// Interface
		public override void Clear()
		{
			base.Clear();

			Name = Default.Name;

			Group.Clear();
			Router.Clear();
			DownloadFolder = FileUtil.GetDownloadFolder();

			lock (SharedFolders)
				SharedFolders.Clear();

			lock (FileMap)
				FileMap.Clear();

			lock (FolderMap)
				FolderMap.Clear();
		}

		public override void Restart()
		{
			base.Restart();

			OnRestart();
		}

		// XML
		public override void Read(XmlReader reader)
		{
			Clear();

			string name = null;
			Enabled = bool.Parse(reader.GetAttribute(XmlTag.Enabled));

			while (reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;

						if (RemoteRouterData.XmlRoot.Equals(name))
							Router.Read(reader);

						break;
					}

					case XmlNodeType.Text:
					{
						switch (name)
						{
							case XmlTag.Id:
							{
								Id = reader.Value;
								break;
							}
							case XmlTag.Name:
							{
								Name =  reader.Value;
								break;
							}
							case XmlTag.GroupImage:
							{
								GroupPath = reader.Value;
								break;
							}
							case XmlTag.DownloadFolder:
							{
								DownloadFolder = reader.Value;
								break;
							}
							case XmlTag.Folder:
							{
								FileData data = new FileData(reader.Value) { Shared = true };
								AddFolder(data);
								break;
							}
						}

						break;
					}

					case XmlNodeType.EndElement:
					{
						if (XmlRoot.Equals(reader.Name))
							return;

						break;
					}
				}
			}
		}

		public override void Write(XmlWriter writer)
		{
			writer.WriteStartElement(XmlRoot);
			writer.WriteAttributeString(XmlTag.Enabled, Enabled.ToString());

			writer.WriteElementString(XmlTag.Id, Id);
			writer.WriteElementString(XmlTag.Name, Name);			
			writer.WriteElementString(XmlTag.GroupImage, GroupPath);
			writer.WriteElementString(XmlTag.DownloadFolder, DownloadFolder);			
			Router.Write(writer);

			SharedFolders.Sort();
			foreach (FileData data in SharedFolders)
				writer.WriteElementString(XmlTag.Folder, data.Path);
			
			writer.WriteEndElement();
		}

		// Modal
		public IEnumerable GetChildren(object parent)
		{
			FileData data = (FileData) parent;
			return (data != null) ? data.GetChildren() : SharedFolders;
		}

		public bool HasChildren(object parent)
		{
			FileData data = (FileData) parent;
			return data.IsDirectory();
		}

		// Folders
		public void AddFolder(FileData data)
		{
			FolderMap.Add(data.Id, data);
			SharedFolders.Add(data);
			SharedFolders.Sort();

			try
			{
				Thread thread = new Thread(new ParameterizedThreadStart(AddFiles)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
				thread.Start(data);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		public void RemoveFolder(FileData data)
		{
			FolderMap.Remove(data.Id);
			SharedFolders.Remove(data);

			try
			{
				Thread thread = new Thread(new ParameterizedThreadStart(RemoveFiles)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
				thread.Start(data);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		public FileData GetFolder(string id)
		{
			FileData data;
			lock (FolderMap)
				FolderMap.TryGetValue(id, out data);

			return data;
		}

#if DEBBUG
		public void PrintFolders()
		{
			lock (FolderMap)
			{
				foreach (string key in FolderMap.Keys)
				{
					FileData data = FolderMap[key];
					Console.WriteLine("{0} = {1}", key, data.Name);
				}
			}
		}
#endif
		// Files
		private void AddFiles(object obj)
		{
			try
			{
				FileData root = (FileData) obj;

				// Folders
				foreach (FileData folder in root.GetFolders())
				{
					lock (FolderMap)
						FolderMap.Add(folder.Id, folder);

					AddFiles(folder);
				}					

				// Files
				foreach (FileData file in root.GetFiles())
				{
					// Do not verify if the file is accessible
					lock (FileMap)
						FileMap.Add(file.Id, file);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			OnFile();
		}

		private void RemoveFiles(object obj)
		{
			try
			{
				FileData root = (FileData) obj;

				// Folders
				foreach (FileData folder in root.GetFolders())
				{
					lock (FolderMap)
						FolderMap.Remove(folder.Id);

					RemoveFiles(folder);
				}					

				// Files
				lock (FileMap)
				{
					foreach (FileData file in root.GetFiles())
						FileMap.Remove(file.Id);
				}				
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			OnFile();
		}

		public FileData GetFile(string id)
		{
			FileData data;
			lock (FileMap)
				FileMap.TryGetValue(id, out data);
			
			return data;
		}

		private bool SearchFilters(FileData file, FileFilter filter)
		{
			return true;
		}

		public Dictionary<string, FileData> SearchFiles(string keyword, FileFilter filter)
		{
			Dictionary<string, FileData> files = new Dictionary<string, FileData>();
			List<string> items = new List<string>();

			// Keywords
			if (keyword.StartsWith("\""))
			{
				int offset = keyword.EndsWith("\"") ? 2 : 1;
				items.Add(keyword.Substring(1, keyword.Length - offset));
			}				
			else
				items.AddRange(keyword.Split(' '));

			// Filters
			FileFormatHandler handler = FileFormat.GetHandler(filter);

			// Files
			foreach (string item in items)
			{
				lock (FileMap)
				{
					foreach (string key in FileMap.Keys)
					{
						FileData data = FileMap[key];

						if (handler(data.Extension))
						{
							string filename = data.Name.ToLower();
							if (filename.Contains(item) && !files.ContainsKey(key))
								files.Add(key, data);
						}						
					}
				}
			}

			return files;
		}

#if DEBBUG
		public void PrintFiles()
		{
			lock (FileMap)
			{
				foreach (string key in FileMap.Keys)
				{
					FileData data = FileMap[key];
					Console.WriteLine("{0} = {1}", key, data.Name);
				}
			}
		}
#endif
	}
}
