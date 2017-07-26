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
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using System;
	using System.Windows.Media.Imaging;

	public sealed class GroupComponent : KeyComponent
	{
		#region Variables
		private string _id;
		private string _path;
		#endregion

		#region Properties
		public string Id
		{
			get { return _id; }
			set
			{
				_id = value;
				OnPropertyChanged("Id");
			}
		}

		public string Name
		{
			get { return !string.IsNullOrEmpty(Path) ? System.IO.Path.GetFileNameWithoutExtension(Path) : Default.Group; }
		}

		public string Path
		{
			get { return _path; }
			set
			{
				_path = value;
				OnPropertyChanged("Path");
				OnPropertyChanged("Name");

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
						Clear();
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}
			}
		}

		public BitmapImage Image
		{
			get
			{
				BitmapImage image = FileUtil.ReadBitmap(Path);
				return (image != null) ? image : Default.NoImage;
			}
		}
		#endregion

		#region Constructors
		public GroupComponent() : this(Default.Id) { }

		public GroupComponent(string id)
		{
			_id = id;
			_path = Default.Path;
		}
		#endregion

		#region Operators
		public static implicit operator JsonGroup(GroupComponent group) { return new JsonGroup(group.Id); }
		#endregion

		#region Interface
		public override void Clear()
		{
			base.Clear();

			_id = Default.Id;
			_path = Default.Path;
			OnPropertyChanged("Path");
			OnPropertyChanged("Name");
		}
		#endregion
	}
}
