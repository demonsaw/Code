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
	using System.Collections.Generic;
	using System.IO;

	public class KeyValueFile : Dictionary<string, string>
	{
		// Constructors
		public KeyValueFile() { }

		// Utility
		public void Load(string path)
		{
			string data = File.ReadAllText(path);
			StringReader stream = new StringReader(data);
			string line = null;

			while ((line = stream.ReadLine()) != null)
			{
				int pos = line.IndexOf('=');
				if (pos > -1)
				{
					string key = line.Substring(0, pos);
					if (ContainsKey(key))
						Remove(key); 
					
					string value = line.Substring(pos + 1, line.Length - (pos + 1));
					Add(key, value);
				}
			}
		}

		public void Save(string path)
		{
			StreamWriter writer = null;

			try
			{
				writer = new StreamWriter(path);

				foreach (KeyValuePair<string, string> entry in this)
				{
					writer.Write(entry.Key);
					writer.Write('=');
					writer.Write(entry.Value);
					writer.Write('\n');
				}	
			}
			finally
			{
				if (writer != null)
					writer.Close();
			}
		}
	}
}

