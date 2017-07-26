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
	using DemonSaw.Json.Object;
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using System;
	using System.Collections.Generic;
	using System.IO;
	using System.Windows.Media.Imaging;

	// Enums
	public enum FileType { None, Drive, Folder, File };

	public class FileData : MachineData
	{
		// Variables
		private static Dictionary<string, BitmapImage> _images = new Dictionary<string, BitmapImage>();

		// Properties		
		public string Path { get; set; }
		public FileType Type { get; set; }
		public bool Shared { get; set; }
		public long Size { get; set; }

		// Properties - Helper
		public string Filename { get { return System.IO.Path.GetFileNameWithoutExtension(Name); } }

		public string Extension
		{
			get
			{
				if (IsFile())
				{
					string ext = System.IO.Path.GetExtension(Name);
					return !string.IsNullOrEmpty(ext) ? ext.Substring(1).ToLower() : Default.Extension;
				}

				return Default.Extension;
			}
		}

		public string Description { get { return IsFile() ? FileUtil.GetDescription(Name) : null; } }

		public BitmapImage Image 
		{ 
			get
			{
				if (IsDrive())
					return Default.DriveBitmap;
				//else if (IsFolder())
				//	return Default.FolderBitmap;
				else
				{
					BitmapImage image = null;
					if (!_images.TryGetValue(Extension, out image))
					{
						image = FileUtil.GetIconBitmap(Path);
						_images.Add(Extension, image);
					}

					return image;
				}
			} 
		}

		// HACK: Properties needed for XAML access
		public bool Directory { get { return Folder || Drive; } }
		public bool Drive { get { return Type == FileType.Drive; } }
		public bool Folder { get { return Type == FileType.Folder; } }
		public bool File { get { return Type == FileType.File; } }

		// Constructors
		public FileData()
		{
			Path = null;
			Type = FileType.None;
			Size = Default.Size; 
		}

		public FileData(string path) : base(path)
		{
			// TODO: The hash should be based on the data, NOT the path!
			//
			Id = Hash.MD5(path);
			Name = System.IO.Path.GetFileName(path);
			if (string.IsNullOrEmpty(Name))
				Name = path;

			SetData(path);
		}

		public FileData(string id, string name)
		{
			Id = id;
			Name = name;

			Path = name;
			Type = (Path.Length <= 3) ? FileType.Drive : FileType.Folder;
			Size = Default.Size;
		}

		public FileData(string id, string name, long size)
		{
			Id = id;
			Name = name;

			Path = name;
			Type = FileType.File;
			Size = size;
		}

		public FileData(FileData data) : base(data)
		{
			Id = data.Id;
			Name = data.Name;

			Path = data.Path; 
			Type = data.Type;
			Size = data.Size;
		}

		public FileData(JsonFile json)
		{
			Id = json.Id;
			Name = json.Name;

			Path = json.Name;
			Type = FileType.File;
			Size = json.Size;
		}

		public FileData(JsonFolder json)
		{
			Id = json.Id;
			Name = json.Name;

			Path = json.Name;
			Type = FileType.Folder;
			Size = Default.Size;
		}

		public FileData(FileSystemInfo info)
		{
			// TODO: The hash should be based on the data, NOT the path!
			//
			Id = Hash.MD5(info.FullName);
			Name = info.Name;

			SetData(info.FullName);
			Created = info.CreationTime;
			Updated = info.LastWriteTime;
		}

		// Overrides
		public bool Equals(FileData data)
		{
			if (ReferenceEquals(null, data))
				return false;

			if (ReferenceEquals(this, data))
				return true;

			return Equals(data.Path, Path);
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
				return false;

			if (ReferenceEquals(this, obj))
				return true;

			if (obj.GetType() != typeof(FileData))
				return false;

			return Equals((FileData) obj);
		}

		public override int GetHashCode() { return Path.GetHashCode(); }


		// Utility
		public override void Clear()
		{
			base.Clear();

			Id = null;
			Name = null;

			Path = null;
			Type = FileType.None;
			Size = Default.Size;
		}

		public bool Verify() { return FileUtil.Valid(Path); }

		// Accessors
		public bool IsDirectory() { return IsFolder() || IsDrive(); }
		public bool IsDrive() { return Type == FileType.Drive; }
		public bool IsFolder() { return Type == FileType.Folder; }
		public bool IsFile() { return Type == FileType.File; }

		public List<FileData> GetFolders()
		{
			List<FileData> children = new List<FileData>();

			if (IsDirectory())
			{
				try
				{
					DirectoryInfo directory = new DirectoryInfo(Path);

					foreach (DirectoryInfo info in directory.GetDirectories())
					{
						if ((info.Attributes & FileAttributes.Directory) == FileAttributes.Directory)
						{
							// Hidden
							if ((info.Attributes & FileAttributes.Hidden) == FileAttributes.Hidden)
								continue;

							// System
							if ((info.Attributes & FileAttributes.System) == FileAttributes.System)
								continue;

							children.Add(new FileData(info));
						}
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}
			}

			return children;
		}

		public List<FileData> GetFiles()
		{
			List<FileData> children = new List<FileData>();

			if (IsDirectory())
			{
				try
				{
					DirectoryInfo directory = new DirectoryInfo(Path);

					foreach (FileInfo info in directory.GetFiles())
					{
						if ((info.Attributes & FileAttributes.Directory) != FileAttributes.Directory)
						{
							// Hidden
							if ((info.Attributes & FileAttributes.Hidden) == FileAttributes.Hidden)
								continue;

							// System
							if ((info.Attributes & FileAttributes.System) == FileAttributes.System)
								continue;

							// Do not verify if the file is accessible
							children.Add(new FileData(info));
						}
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}
			}

			return children;
		}

		public List<FileData> GetChildren()
		{
			List<FileData> children = new List<FileData>();

			if (IsDirectory())
			{
				try
				{
					DirectoryInfo directory = new DirectoryInfo(Path);

					// Directories
					foreach (DirectoryInfo info in directory.GetDirectories())
					{
						if ((info.Attributes & FileAttributes.Directory) == FileAttributes.Directory)
							children.Add(new FileData(info));
					}

					// Files
					foreach (FileInfo info in directory.GetFiles())
					{
						if ((info.Attributes & FileAttributes.Directory) != FileAttributes.Directory)
							children.Add(new FileData(info));
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}
			}

			return children;
		}

		// Mutators
		protected void SetData(string path)
		{
			try
			{
				Path = path;
				FileAttributes attr = System.IO.File.GetAttributes(path);

				if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
				{
					Type = (Path.Length <= 3) ? FileType.Drive : FileType.Folder;
					Size = Default.Size;
				}
				else
				{
					Type = FileType.File;
					FileInfo info = new FileInfo(path);
					Size = info.Length;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				Size = Default.Size;
			}
		}
	}
}
