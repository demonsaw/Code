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
	using System.Xml;

	public sealed class ConfigFile : XmlFile
	{
		// Constants
		public const string XmlFilename = "demonsaw_server.xml";

		// Properties - Helper
		private static LogController LogController { get { return Global.LogController; } }
		private static ServerController Server { get { return Global.ServerController; } }
		private static EntityObservableList Servers { get { return Global.ServerController.List; } }

		// Constructors
		public ConfigFile() : base(XmlFilename) { }
		public ConfigFile(XmlReader reader) : base(XmlFilename) { Read(reader); }

		#region Interface
		public void Update()
		{
			Server.Update();
		}
		#endregion

		#region Utility
		public override void Save()
		{
			Servers.Sort(delegate(Entity entity1, Entity entity2)
			{
				ServerComponent server1 = entity1.Get<ServerComponent>();
				ServerComponent server2 = entity2.Get<ServerComponent>();
				return server1.Name.CompareTo(server2.Name);
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
							case XmlTag.Servers:
							{
								ReadServers(reader);
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

		private void ReadServers(XmlReader reader)
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
							case XmlTag.Server:
							{
								ReadServer(reader);
								break;
							}							
						}

						break;
					}
					case XmlNodeType.EndElement:
					{
						if (XmlTag.Servers.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}
		}

		private void ReadServer(XmlReader reader)
		{
			if (reader.IsEmptyElement)
				return;

			bool loop = true;
			string name = null;

			// Components
			Entity entity = new Entity();
			ServerComponent server = new ServerComponent();
			ServerOptionsComponent options = new ServerOptionsComponent();

			server.Enabled = bool.Parse(reader.GetAttribute(XmlTag.Enabled));

			while (loop && reader.Read())
			{
				switch (reader.NodeType)
				{
					case XmlNodeType.Element:
					{
						name = reader.Name;

						switch (name)
						{
							case XmlTag.Options:
							{								
								ReadOptions(reader, ref options);
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
						if (XmlTag.Server.Equals(reader.Name))
							loop = false;

						break;
					}
				}
			}

			// Components
			entity.Add(server);
			entity.Add(options);

			ServerStatusComponent status = new ServerStatusComponent();
			entity.Add(status);

			MachineComponent machine = new ServerMachineComponent();
			entity.Add(machine);

			SessionMapComponent sessions = new SessionMapComponent();
			entity.Add(sessions);
#if DEBUG
			LogComponent log = new LogComponent(LogController);
			entity.Add(log);
#endif
			Server.Add(entity);
		}

		private void ReadOptions(XmlReader reader, ref ServerOptionsComponent options)
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
							case XmlTag.Ping:
							{
								options.Ping = bool.Parse(reader.Value);
								break;
							}
							case XmlTag.Info:
							{
								options.Info = bool.Parse(reader.Value);
								break;
							}
							case XmlTag.ChunkSize:
							{
								options.ChunkSize = int.Parse(reader.Value);
								break;
							}
							case XmlTag.MaxChunks:
							{
								options.MaxChunks = int.Parse(reader.Value);
								break;
							}
							case XmlTag.MaxTransfers:
							{
								options.MaxTransfers = int.Parse(reader.Value);
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

		public override void Write(XmlWriter writer)
		{
			writer.WriteStartDocument();
			writer.WriteStartElement(XmlTag.DemonSaw);
			writer.WriteAttributeString(XmlTag.Version, Version.ToString());

			WriteServers(writer);

			writer.WriteEndElement();
			writer.WriteEndDocument();
		}

		private void WriteServers(XmlWriter writer)
		{
			writer.WriteStartElement(XmlTag.Servers);

			lock (Servers)
			{
				foreach (Entity entity in Servers)
					WriteServer(writer, entity);
			}

			writer.WriteEndElement();
		}

		private void WriteServer(XmlWriter writer, Entity entity)
		{
			writer.WriteStartElement(XmlTag.Server);

			ServerComponent server = entity.Get<ServerComponent>();
			writer.WriteAttributeString(XmlTag.Enabled, server.Enabled.ToString());

			writer.WriteElementString(XmlTag.Id, entity.Id);
			writer.WriteElementString(XmlTag.Name, server.Name);
			writer.WriteElementString(XmlTag.Passphrase, server.Passphrase);
			writer.WriteElementString(XmlTag.Address, server.Address.ToString());
			writer.WriteElementString(XmlTag.Port, server.Port.ToString());

			WriteOptions(writer, entity);

			writer.WriteEndElement();
		}

		private void WriteOptions(XmlWriter writer, Entity entity)
		{
			ServerOptionsComponent options = entity.Get<ServerOptionsComponent>();

			writer.WriteStartElement(XmlTag.Options);

			writer.WriteElementString(XmlTag.Ping, options.Ping.ToString());
			writer.WriteElementString(XmlTag.Info, options.Info.ToString());
			writer.WriteElementString(XmlTag.ChunkSize, options.ChunkSize.ToString());
			writer.WriteElementString(XmlTag.MaxChunks, options.MaxChunks.ToString());
			writer.WriteElementString(XmlTag.MaxTransfers, options.MaxTransfers.ToString());

			writer.WriteEndElement();
		}
		#endregion
	}
}
