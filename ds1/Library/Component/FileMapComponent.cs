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

namespace DemonSaw.Component
{
	using DemonSaw.IO;
	using DemonSaw.Json.Object;
	using System.Collections.Generic;

	public sealed class FileMapComponent : MapComponent<string, FileComponent>
	{
		#region Utility
		public List<FileComponent> Search(JsonSearch jsonSearch)
		{
			return Search(jsonSearch.Keyword, jsonSearch.Filter);
		}

		public List<FileComponent> Search(string keyword, FileFilter filter)
		{
			List<FileComponent> list = new List<FileComponent>();
			List<string> keywords = new List<string>();

			// Keywords
			if (keyword.StartsWith("\""))
			{
				int offset = keyword.EndsWith("\"") ? 2 : 1;
				keywords.Add(keyword.Substring(1, keyword.Length - offset));
			}
			else
				keywords.AddRange(keyword.Split(' '));

			// Filters
			FileFormatHandler handler = FileFormat.GetHandler(filter);

			// Files
			IEnumerator<string> it = keywords.GetEnumerator();

			while (it.MoveNext())
			{
				keyword = (string) it.Current;

				lock (Map)
				{
					foreach (string id in Map.Keys)
					{
						FileComponent file = Get(id);

						if (handler(file.Extension))
						{
							string filename = file.Name.ToLower();
							if (filename.Contains(keyword))
								list.Add(file);
						}
					}

				}
			}

			return list;
		}
		#endregion
	}
}
