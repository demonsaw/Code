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

namespace DemonSaw.IO
{
	using DemonSaw.Component;
	using DemonSaw.Controller;
	using DemonSaw.Entity;
	using DemonSaw.Xml;
	using System;
	using System.Collections.Generic;
	using System.Xml;

	public class ConfigFile : XmlFile
	{
		// Constants
		private const string XmlFilename = "demonsaw_client.xml";

		// Properties
		private static LogController LogController { get { return Global.LogController; } }
		private static ClientController ClientController { get { return Global.ClientController; } }
		private static FileController FileController { get { return Global.FileController; } }
		private static SearchController SearchController { get { return Global.SearchController; } }
		private static DownloadController DownloadController { get { return Global.DownloadController; } }
		private static UploadController UploadController { get { return Global.UploadController; } }

		private static EntityObservableList Clients { get { return ClientController.List; } }

		// Constructors
		public ConfigFile() : base(XmlFilename) { }
		public ConfigFile(XmlReader reader) : base(XmlFilename) { Read(reader); }

		#region Interface
		public void Update()
		{
			ClientController.Update();
		}
		#endregion

		#region Utility
		public override void Save()
		{
			Clients.Sort(delegate(Entity entity1, Entity entity2)
			{
				ClientComponent client1 = entity1.Get<ClientComponent>();
				ClientComponent client2 = entity2.Get<ClientComponent>();
				return client1.Name.CompareTo(client2.Name);
			});

			base.Save();
		}

		public override void Read(XmlReader reader)
		{
			Init();

			while (reader.Read())
			{
				string name = reader.Name;

				switch (reader.NodeType)
				{
					case XmlNodeType.DocumentType:
					{
						if (!XmlTag.DemonSaw.Equals(name))
							throw new Exception("Invalid tag");

						Version = int.Parse(reader.GetAttribute(XmlTag.Version));

						break;
					}
					case XmlNodeType.Element:
					{
						switch (name)
						{
							case XmlTag.Clients:
							{
								ReadClients(reader);
								break;
							}	
						}

						break;
					}
					case XmlNodeType.EndElement:
					{
						if (XmlTag.DemonSaw.Equals(reader.Name))
							Update();

						break;
					}
				}
			}
		}

		private void ReadClients(XmlReader reader)
		{
			if (reader.IsEmptyElement)
				return;

			bool loop = true;
			string name = null;

			while (loop && reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;

						switch (name)
						{
							case XmlTag.Client:
							{
								ReadClient(reader);
								break;
							}
						}

						break;
					}
					case XmlNodeType.EndElement:
					{
						if (XmlTag.Clients.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}
		}

		private void ReadClient(XmlReader reader)
		{
			if (reader.IsEmptyElement)
				return;

			bool loop = true;
			string name = null;

			// Components
			Entity entity = new Entity();
			ClientComponent client = new ClientComponent();			
			ClientOptionsComponent options = new ClientOptionsComponent();

			GroupComponent group = new GroupComponent();
			ServerComponent server = new ServerComponent();
			List<string> paths = new List<string>();

			client.Enabled = bool.Parse(reader.GetAttribute(XmlTag.Enabled));

			while (loop && reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;

						switch (name)
						{
							case XmlTag.Router:
							{
								ReadRouter(reader, ref server);
								break;
							}
							case XmlTag.Options:
							{
								ReadOptions(reader, ref options);
								break;
							}
							case XmlTag.Folders:
							{
								ReadFolders(reader, ref paths);								
								break;
							}
						}

						break;
					}

					case XmlNodeType.Text:
					{
						switch (name)
						{
							case XmlTag.Id:
							{
								entity.Id = reader.Value;
								break;
							}
							case XmlTag.Name:
							{
								client.Name = reader.Value;
								break;
							}
							case XmlTag.Download:
							{
								client.Download = reader.Value;
								break;
							}
							case XmlTag.Group:
							{
								group.Path = reader.Value;
								break;
							}
						}

						break;
					}

					case XmlNodeType.EndElement:
					{
						if (XmlTag.Client.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}

			// Components
			entity.Add(client);
			entity.Add(options);
			entity.Add(group);
			entity.Add(server);

			FolderListComponent folderList = new FolderListComponent();
			entity.Add(folderList);

			FileMapComponent fileMap = new FileMapComponent();
			entity.Add(fileMap);

			FolderMapComponent folderMap = new FolderMapComponent();
			entity.Add(folderMap);

			ClientMachineComponent machine = new ClientMachineComponent();
			entity.Add<MachineComponent>(machine);

			SessionComponent session = new SessionComponent();
			entity.Add(session);

			FileComponent file = new FileComponent();
			entity.Add(file);

			SearchListComponent search = new SearchListComponent(SearchController.Dispatcher);
			entity.Add(search);

			BrowseComponent browse = new BrowseComponent();
			entity.Add(browse);
			
			DownloadListComponent download = new DownloadListComponent(DownloadController.Dispatcher);
			entity.Add(download);

			UploadListComponent upload = new UploadListComponent(UploadController.Dispatcher);
			entity.Add(upload);
#if DEBUG
			LogComponent log = new LogComponent(LogController);
			entity.Add(log);
#endif
			ClientController.Add(entity);

			foreach (string path in paths)
			{
				FileComponent shared = new FileComponent(path) { Owner = entity };
				folderList.Add(shared);
			}							
		}

		private void ReadRouter(XmlReader reader, ref ServerComponent server)
		{
			if (reader.IsEmptyElement)
				return;
					
			bool loop = true;
			string name = null;

			while (loop && reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;
						break;
					}
					case XmlNodeType.Text:
					{
						switch (name)
						{
							case XmlTag.Name:
							{
								server.Name = reader.Value;
								break;
							}
							case XmlTag.Passphrase:
							{
								server.Passphrase = reader.Value;
								break;
							}
							case XmlTag.Address:
							{
								server.Address = reader.Value;
								break;
							}
							case XmlTag.Port:
							{
								server.Port = int.Parse(reader.Value);
								break;
							}
						}

						break;
					}
					case XmlNodeType.EndElement:
					{
						if (XmlTag.Router.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}
		}

		private void ReadOptions(XmlReader reader, ref ClientOptionsComponent options)
		{
			if (reader.IsEmptyElement)
				return;

			bool loop = true;
			string name = null;

			while (loop && reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;
						break;
					}
					case XmlNodeType.Text:
					{
						switch (name)
						{
							case XmlTag.MaxDownloads:
							{
								options.MaxDownloads = int.Parse(reader.Value);
								break;
							}
							case XmlTag.MaxUploads:
							{
								options.MaxUploads = int.Parse(reader.Value);
								break;
							}
						}

						break;
					}
					case XmlNodeType.EndElement:
					{
						if (XmlTag.Options.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}
		}

		private void ReadFolders(XmlReader reader, ref List<string> paths)
		{
			if (reader.IsEmptyElement)
				return;

			bool loop = true;
			string name = null;

			while (loop && reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;
						break;
					}
					case XmlNodeType.Text:
					{
						switch (name)
						{
							case XmlTag.Path:
							{
								paths.Add(reader.Value);
								break;
							}
						}

						break;
					}
					case XmlNodeType.EndElement:
					{
						if (XmlTag.Folders.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}
		}

		public override void Write(XmlWriter writer)
		{
			writer.WriteStartDocument();
			writer.WriteStartElement(XmlTag.DemonSaw);
			writer.WriteAttributeString(XmlTag.Version, Version.ToString());

			WriteClients(writer);

			writer.WriteEndElement();
			writer.WriteEndDocument();
		}

		private void WriteClients(XmlWriter writer)
		{
			writer.WriteStartElement(XmlTag.Clients);

			lock (Clients)
			{
				foreach (Entity entity in Clients)
					WriteClient(writer, entity);
			}

			writer.WriteEndElement();
		}

		private void WriteClient(XmlWriter writer, Entity entity)
		{
			writer.WriteStartElement(XmlTag.Client);

			ClientComponent client = entity.Get<ClientComponent>();
			writer.WriteAttributeString(XmlTag.Enabled, client.Enabled.ToString());
			writer.WriteElementString(XmlTag.Id, entity.Id);
			writer.WriteElementString(XmlTag.Name, client.Name);
			writer.WriteElementString(XmlTag.Download, client.Download);

			GroupComponent group = entity.Get<GroupComponent>();
			writer.WriteElementString(XmlTag.Group, group.Path);

			WriteRouter(writer, entity);
			WriteOptions(writer, entity);
			WriteFolders(writer, entity);

			writer.WriteEndElement();
		}

		private void WriteRouter(XmlWriter writer, Entity entity)
		{
			ServerComponent server = entity.Get<ServerComponent>();

			writer.WriteStartElement(XmlTag.Router);

			writer.WriteElementString(XmlTag.Name, server.Name); 
			writer.WriteElementString(XmlTag.Passphrase, server.Passphrase);
			writer.WriteElementString(XmlTag.Address, server.Address.ToString());
			writer.WriteElementString(XmlTag.Port, server.Port.ToString());

			writer.WriteEndElement();
		}

		private void WriteOptions(XmlWriter writer, Entity entity)
		{
			ClientOptionsComponent options = entity.Get<ClientOptionsComponent>();

			writer.WriteStartElement(XmlTag.Options);

			writer.WriteElementString(XmlTag.MaxDownloads, options.MaxDownloads.ToString());
			writer.WriteElementString(XmlTag.MaxUploads, options.MaxUploads.ToString());

			writer.WriteEndElement();
		}

		private void WriteFolders(XmlWriter writer, Entity entity)
		{
			FolderListComponent files = entity.Get<FolderListComponent>();

			writer.WriteStartElement(XmlTag.Folders);

			foreach (FileComponent file in files)
				writer.WriteElementString(XmlTag.Path, file.Path);

			writer.WriteEndElement();
		}
		#endregion
	}
}
