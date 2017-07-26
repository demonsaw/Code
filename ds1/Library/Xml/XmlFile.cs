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

namespace DemonSaw.Xml
{
	using DemonSaw.Utility;
	using System;
	using System.IO;
	using System.Xml;

	public abstract class XmlFile : Demon, XmlParseable
	{
		// Constants
		public const string _folder = "demonsaw";

		// Properties
		public int Version { get; protected set; }
		public string Filename { get; private set; }

		// Properties - Helper
		public string Path { get { return System.IO.Path.Combine(Folder, Filename); } }
		public string Folder { get { return System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), _folder); } }	

		// Constructors
		protected XmlFile(string filename)
		{
			Version = Constant.Version; 
			Filename = filename;
		}

		// Interface
		public virtual void Init() { Version = Constant.Version; }
		public abstract void Read(XmlReader reader);
		public abstract void Write(XmlWriter writer);

		// Utility
		public virtual void Load()
		{
			if (File.Exists(Path))
			{
				XmlReader reader = XmlReader.Create(Path);
				Read(reader);
				reader.Close();
			}
		}

		public virtual void Save()
		{
			if (!Directory.Exists(Folder))
				Directory.CreateDirectory(Folder);

			XmlWriter writer = XmlWriter.Create(Path);
			Write(writer);
			writer.Close();
		}
	}
}
