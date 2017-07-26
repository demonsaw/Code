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
	using DemonSaw.Entity;
	using DemonSaw.IO;
	using DemonSaw.Json.Object;
	using DemonSaw.Security;
	using DemonSaw.Utility;
	using DemonSaw.Wpf.TreeList;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.IO;
	using System.Windows.Media.Imaging;

	public enum FileType { None, File, Folder, Drive };

	public class FileComponent : EntityComponent, ITreeModel, IComparable<FileComponent>
	{
		#region Variables
		protected string _id;
		protected string _path;
		protected long _size;
		protected FileType _type;
		#endregion

		#region Properties
		public TreeNode Node { get; set; }

		public string Id
		{
			get { return _id; }
			set
			{
				_id = value;
				OnPropertyChanged("Id");
			}
		}

		public string Path
		{
			get { return _path; }
			set
			{
				_path = value;
				OnPropertyChanged("Path");
			}
		}

		public long Size
		{
			get { return _size; }
			set
			{
				_size = value;
				OnPropertyChanged("Size");
			}
		}

		public FileType Type
		{
			get { return _type; }
			set
			{
				_type = value;
				OnPropertyChanged("Type");
			}
		}

		public string Name { get { return System.IO.Path.GetFileName(Path); } }
		public virtual string Description { get { return File ? FileUtil.GetDescription(Name) : Default.Description; } }

		public string Extension
		{
			get
			{
				string ext = System.IO.Path.GetExtension(Path);
				return !string.IsNullOrEmpty(ext) ? ext.Substring(1) : Default.Extension;
			}
		}

		public virtual BitmapImage Image
		{
			get
			{
				switch (Type)
				{
					case FileType.File:
					{
						return FileUtil.GetFileBitmap(Name, Extension);
					}
					case FileType.Folder:
					{
						return FileUtil.GetFolderBitmap(Name);
					}
					case FileType.Drive:
					{
						return Default.DriveBitmap;
					}
				}

				return null;
			}
		}

		public bool Exists { get { return File ? FileUtil.Exists(Path) : DirectoryUtil.Exists(Path); } }

		public bool None { get { return Type == FileType.None; } }
		public bool File { get { return Type == FileType.File; } }
		public bool Folder { get { return Type == FileType.Folder; } }
		public bool Drive { get { return Type == FileType.Drive; } }
		#endregion

		#region Constructors
		public FileComponent() { }
		public FileComponent(FileComponent file)
		{
			_id = file.Id;
			_path = file.Path;
			_size = file.Size;
			_type = file.Type;
		}

		public FileComponent(string path) : this(Hash.MD5(path), path) { }
		public FileComponent(string id, string path)
		{
			_id = id;
			_path = path;
			_type = FileType.Folder;
		}

		public FileComponent(string path, long size) : this(Hash.MD5(path), path, size) { }
		public FileComponent(string id, string path, long size)
		{
			_id = id;
			_path = path;
			_size = size;
			_type = FileType.File;
		}
		#endregion

		#region System
		public override string ToString()
		{
			return Name;
		}

		public int CompareTo(FileComponent file)
		{
			return Path.CompareTo(file.Path);
		}

		public bool Equals(FileComponent file)
		{
			if (ReferenceEquals(null, file))
				return false;

			if (ReferenceEquals(this, file))
				return true;

			return Equals(file.Path, Path);
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
				return false;

			if (ReferenceEquals(this, obj))
				return true;

			if (obj.GetType() != typeof(FileComponent))
				return false;

			return Equals((FileComponent) obj);
		}

		public override int GetHashCode() { return Path.GetHashCode(); }
		#endregion

		#region Operators
		public static implicit operator JsonFile(FileComponent file) { return new JsonFile(file.Id, file.Name, file.Size); }
		#endregion

		#region Utility
		public void Open()
		{
			FileUtil.Execute(Path);
		}

		public List<FileComponent> Folders
		{
			get
			{
				if (File)
					return null;

				List<FileComponent> list = new List<FileComponent>();

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

							try
							{
								FileComponent file = new FileComponent(info.FullName) { Owner = Owner };
								list.Add(file);
							}
							catch (Exception) { }
						}
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}

				return list;
			}
		}

		public List<FileComponent> Files
		{
			get
			{
				if (File)
					return null;

				List<FileComponent> list = new List<FileComponent>();

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

							try
							{
								FileComponent file = new FileComponent(info.FullName, info.Length) { Owner = Owner };
								list.Add(file);
							}
							catch (Exception) { }
						}
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}

				return list;
			}
		}

		public List<FileComponent> Children
		{
			get
			{
				if (File)
					return null;

				List<FileComponent> list = new List<FileComponent>();
				try
				{
					list.AddRange(Folders);
					list.AddRange(Files);
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
				}

				return list;
			}
		}
		#endregion

		#region Model
		public virtual IEnumerable GetChildren(object obj)
		{
			FileComponent file = (FileComponent) obj;

			if (file != null)
			{
				return file.Children;
			}
			else
			{
				FolderListComponent folders = Owner.Get<FolderListComponent>();
				return folders.List;
			}
		}

		public virtual bool HasChildren(object obj)
		{
			FileComponent file = (FileComponent) obj;
			return !file.File;
		}
		#endregion
	}
}
