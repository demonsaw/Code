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
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using System;
	using System.Windows.Media.Imaging;

	public class GroupData : KeyData
	{
		// Variables
		private string _path;

		// Properties
		public string Id { get; private set; }

		// Properties - Helper
		public string Name
		{
			get { return !string.IsNullOrEmpty(Path) ? System.IO.Path.GetFileNameWithoutExtension(Path) : "None"; }
		}

		public BitmapImage Image
		{
			get
			{ 
				BitmapImage image = FileUtil.ReadBitmap(Path);
				return (image != null) ? image : Default.NoImage; 
			}
		}

		public string Path
		{
			get { return _path; }
			set
			{
				_path = value;

				try
				{
					// Image
					if (FileUtil.Exists(_path))
					{
						byte[] data = FileUtil.Read(_path);

						// Key
						Key.Set(SecurityUtil.Seed);
						for (int i = 0; i < (data.Length - (data.Length % SecurityUtil.KeySize)); i += SecurityUtil.KeySize)
						{
							for (int j = 0; j < SecurityUtil.KeySize; ++j)
								Key.Data[j] ^= data[i + j];
						}

						// Id
						Id = SecurityUtil.CreateKeyString(Key.ToString());
					}
					else
					{
						// No image
						base.Clear();
						Id = null;
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}
			}
		}

		// Constructors
		public GroupData() { }
		public GroupData(string path, Key key) : base(key) { Path = path; }

		// Interface
		public override void Clear()
		{
			base.Clear();

			Id = null;
			Path = null;
		}
	}
}
