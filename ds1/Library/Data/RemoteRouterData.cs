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
	using DemonSaw.Utility;
	using DemonSaw.Xml;
	using System.Xml;

	public class RemoteRouterData : DemonRouterData
	{		
		// Variables
		public int _clientCount;
		public int _groupCount;

		// Properties - Helper
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

		// Properties - Static
		public static RemoteRouterData DefaultData { get { return new RemoteRouterData(); } }

		// Constructors
		public RemoteRouterData()
		{
			Name = Default.Name;
			ClientCount = Default.ClientCount;
			GroupCount = Default.GroupCount;
		}

		public RemoteRouterData(XmlReader reader) : this() { Read(reader); }

		// Interface
		public override void Clear()
		{
			base.Clear();

			ClientCount = Default.ClientCount;
			GroupCount = Default.GroupCount;
		}

		// XML
		public override void Read(XmlReader reader)
		{
			Clear();

			string name = null;

			while (reader.Read())
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
								Name = reader.Value;
								break;
							}

							case XmlTag.Address:
							{
								Address = reader.Value;
								break;
							}

							case XmlTag.Port:
							{
								Port = int.Parse(reader.Value);
								break;
							}

							case XmlTag.Passphrase:
							{
								Passphrase = reader.Value;
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
			writer.WriteElementString(XmlTag.Name, Name);
			writer.WriteElementString(XmlTag.Address, Address.ToString());
			writer.WriteElementString(XmlTag.Port, Port.ToString());
			writer.WriteElementString(XmlTag.Passphrase, Passphrase); 
			writer.WriteEndElement();
		}
	}
}
