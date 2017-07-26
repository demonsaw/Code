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
	using DemonSaw.Utility;
	using Microsoft.Win32;
	using System;
	using System.Collections.Generic;
	using System.Drawing;
	using System.IO;
	using System.Reflection;
	using System.Runtime.InteropServices;
	using System.Text;
	using System.Windows.Media.Imaging;

	public static class FileUtil
	{
		#region Natives
		private const int SHGFI_ICON = 0x00100;     // get icon
		private const int SHGFI_DISPLAYNAME = 0x00200;     // get display name
		private const int SHGFI_TYPENAME = 0x00400;     // get type name
		private const int SHGFI_ATTRIBUTES = 0x00800;     // get attributes
		private const int SHGFI_ICONLOCATION = 0x01000;     // get icon location
		private const int SHGFI_EXETYPE = 0x02000;     // return exe type
		private const int SHGFI_SYSICONINDEX = 0x04000;     // get system icon index
		private const int SHGFI_LINKOVERLAY = 0x08000;     // put a link overlay on icon
		private const int SHGFI_SELECTED = 0x10000;     // show icon in selected state
		private const int SHGFI_ATTR_SPECIFIED = 0x20000;     // get only specified attributes
		private const int SHGFI_LARGEICON = 0x00000;     // get large icon
		private const int SHGFI_SMALLICON = 0x00001;     // get small icon
		private const int SHGFI_OPENICON = 0x00002;     // get open icon
		private const int SHGFI_SHELLICONSIZE = 0x00004;     // get shell size icon
		private const int SHGFI_PIDL = 0x00008;     // pszPath is a pidl
		private const int SHGFI_USEFILEATTRIBUTES = 0x00010;     // use passed dwFileAttribute
		//if (_WIN32_IE >= 0x0500)
		private const int SHGFI_ADDOVERLAYS = 0x00020;     // apply the appropriate overlays
		private const int SHGFI_OVERLAYINDEX = 0x00040;     // Get the index of the overlay

		private const int FILE_ATTRIBUTE_NORMAL = 0x00080;     // Normal file
		private const int FILE_ATTRIBUTE_DIRECTORY = 0x00010;     // Directory

		private const int MAX_PATH = 260;

		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		private struct SHFILEINFO
		{
			public IntPtr hIcon;
			public int iIcon;
			public int dwAttributes;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_PATH)]
			public string szDisplayName;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
			public string szTypeName;
		}

		private const int SW_SHOWNORMAL = 1;

		[DllImport("shell32.dll", CharSet = CharSet.Auto)]
		private static extern IntPtr ShellExecute(int hwnd, string lpOperation, string lpFile,
			string lpParameters, string lpDirectory, int nShowCmd);

		[DllImport("shell32.dll", CharSet = CharSet.Auto)]
		private static extern IntPtr SHGetFileInfo(string pszPath, int dwFileAttributes,
			out SHFILEINFO psfi, int cbFileInfo, int uFlags);
		#endregion

		#region variables
		private static Dictionary<string, BitmapImage> _images = new Dictionary<string, BitmapImage>();
		#endregion

		// Utility
		public static void Create(string path, long size = 0)
		{
			using (FileStream stream = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write, FileShare.ReadWrite))
			{
				stream.SetLength(size);
				stream.Close();
			}
		}

		public static void Delete(string path)
		{
			File.Delete(path);
		}

		public static bool Exists(string path)
		{
			return File.Exists(path);
		}

		public static bool Valid(string path)
		{
			try
			{
				using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
				{
					using (BinaryReader reader = new BinaryReader(stream))
					{
						reader.ReadByte();
						reader.Close();
					}

					stream.Close();
					return true;
				}
			}
			catch (System.Exception ex)
			{
				Console.WriteLine(ex.Message);
				return false;
			}
		}

		public static byte[] Read(string path, long size = 0, long offset = 0)
		{
			byte[] data = null;

			using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
			{
				if (offset > 0)
					stream.Seek(offset, SeekOrigin.Begin);

				using (BinaryReader reader = new BinaryReader(stream))
				{
					long length = stream.Length;
					size = (size > 0) ? Math.Min(size, length - offset) : length;
					data = reader.ReadBytes((int) size);
					reader.Close();
				}

				stream.Close();
			}

			return data;
		}

		public static Image ReadImage(string path)
		{
			Image image = Default.NoImageEmbedded;

			try
			{
				using (MemoryStream stream = new MemoryStream(File.ReadAllBytes(path)))
				{
					image = Image.FromStream(stream);
					stream.Close();
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			return image;
		}

		public static BitmapImage ReadBitmap(string path)
		{
			try
			{
				if (Exists(path))
				{
					BitmapImage image = new BitmapImage();
					image.BeginInit();
					image.UriSource = new Uri(path);
					image.CacheOption = BitmapCacheOption.OnLoad;
					image.EndInit();
					return image;
				}				
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);				
			}

			return null;
		}

		public static void Write(string path, string data, long offset = 0) { Write(path, Encoding.ASCII.GetBytes(data), offset); }
		public static void Write(string path, byte[] data, long offset = 0)
		{
			using (FileStream stream = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write, FileShare.ReadWrite))
			{
				stream.Seek(offset, SeekOrigin.Begin);

				using (BinaryWriter writer = new BinaryWriter(stream))
				{
					writer.Write(data);
					writer.Close();
				}

				stream.Close();
			}
		}

		public static void Append(string path, string data) { Append(path, Encoding.ASCII.GetBytes(data)); }
		public static void Append(string path, byte[] data)
		{
			using (FileStream stream = new FileStream(path, FileMode.Append, FileAccess.Write, FileShare.ReadWrite))
			{
				using (BinaryWriter writer = new BinaryWriter(stream))
				{
					writer.Write(data);
					writer.Close();
				}

				stream.Close();
			}
		}

		public static void Clear(string path)
		{
			using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Write, FileShare.ReadWrite))
			{
				stream.SetLength(0);
				stream.Close();
			}
		}

		// Accessors
		public static string GetSize(long size)
		{
			if (size == Default.Size)
				return null;

			int[] limits = new int[] { 1024 * 1024 * 1024, 1024 * 1024, 1024 };
			string[] units = new string[] { "GB", "MB", "KB" };

			for (int i = 0; i < limits.Length; i++)
			{
				if (size >= limits[i])
					return string.Format("{0:#,##0.00} " + units[i], ((double) size / limits[i]));
			}

			return string.Format("{0} bytes", size);
		}

		public static string GetSpeed(long size)
		{
			if (size == 0)
				return "0 KB/sec";

			int[] limits = new int[] { 1000 * 1000 * 1000, 1000 * 1000, 1000 };
			string[] units = new string[] { "GB/s", "MB/s", "KB/s" };			

			for (int i = 0; i < limits.Length; i++)
			{
				if (size >= limits[i])
					return string.Format("{0:#,##0.00} " + units[i], ((double) size / limits[i]));				
			}

			return "0 KB/sec";
		}

		public static String GetDescription(string path)
		{
			SHFILEINFO shfi = new SHFILEINFO();
			int flags = SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES;
			IntPtr result = SHGetFileInfo(path, 0, out shfi, Marshal.SizeOf(shfi), flags);
			if (result.ToInt32() == 0)
				return String.Empty;
			else
				return shfi.szTypeName;
		}

		public static Image GetAssemblyImage(string name)
		{
			//string[] names = GetType().Assembly.GetManifestResourceNames();
			//foreach (string name in names)
			//	System.Console.WriteLine(name);
			//
			Assembly asm = Assembly.GetExecutingAssembly();
			using (Stream resStream = asm.GetManifestResourceStream(string.Format("DemonSaw.Resources.{0}", name)))
				return new Bitmap(resStream);
		}

		public static BitmapImage GetResourceBitmap(string name, string folder = null)
		{
			string path = (folder != null) ? Path.Combine(folder, name) : name;
			path = string.Format("pack://application:,,/Resources/{0}", path);

			BitmapImage image = new BitmapImage();
			image.BeginInit();
			image.UriSource = new Uri(path);
			image.CacheOption = BitmapCacheOption.OnLoad;
			image.EndInit();
			return image;
		}

		public static Icon GetFolderIcon(string path)
		{
			int flags = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES;
			int fileAttributes = FILE_ATTRIBUTE_DIRECTORY;

			SHFILEINFO shfi = new SHFILEINFO();
			IntPtr result = SHGetFileInfo(path, fileAttributes, out shfi, Marshal.SizeOf(shfi), flags);
			if (result.ToInt32() == 0)
				return null;
			else
				return Icon.FromHandle(shfi.hIcon);
		}

		public static BitmapImage GetFolderBitmap(string path)
		{
			Icon icon = GetFolderIcon(path);
			return IconToBitmap(icon);
		}

		public static Icon GetFileIcon(string path)
		{
			int flags = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES;
			int fileAttributes = FILE_ATTRIBUTE_NORMAL;

			SHFILEINFO shfi = new SHFILEINFO();
			IntPtr result = SHGetFileInfo(path, fileAttributes, out shfi, Marshal.SizeOf(shfi), flags);
			if (result.ToInt32() == 0)
				return null;
			else
				return Icon.FromHandle(shfi.hIcon);
		}

		public static BitmapImage GetFileBitmap(string path, string ext = null)
		{
			if (ext == null)
				ext = Path.GetExtension(path);

			BitmapImage image;
			if (!_images.TryGetValue(ext, out image))
			{
				Icon icon = GetFileIcon(path);
				image = IconToBitmap(icon);
				_images.Add(ext, image);
			}

			return image;
		}

		public static Icon GetIcon(string path, bool isSmallImage = true, bool useFileType = true)
		{
			int flags = SHGFI_ICON;
			if (isSmallImage)
				flags |= SHGFI_SMALLICON;

			int fileAttributes = 0;
			if (useFileType)
			{
				flags |= SHGFI_USEFILEATTRIBUTES;

				try
				{
					if (Directory.Exists(path))
						fileAttributes = FILE_ATTRIBUTE_DIRECTORY;
					else
						fileAttributes = FILE_ATTRIBUTE_NORMAL;
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.Message);
					fileAttributes = FILE_ATTRIBUTE_NORMAL;
				}
			}

			SHFILEINFO shfi = new SHFILEINFO();
			IntPtr result = SHGetFileInfo(path, fileAttributes, out shfi, Marshal.SizeOf(shfi), flags);
			if (result.ToInt32() == 0)
				return null;
			else
				return Icon.FromHandle(shfi.hIcon);
		}

		public static BitmapImage GetIconBitmap(string path, bool isSmallImage = true, bool useFileType = true)
		{
			Icon icon = GetIcon(path, isSmallImage, useFileType);
			return IconToBitmap(icon);
		}

		private static BitmapImage IconToBitmap(Icon icon)
		{
			if (icon == null)
				return null;

			Bitmap bitmap = icon.ToBitmap();
			MemoryStream stream = new MemoryStream();
			bitmap.Save(stream, System.Drawing.Imaging.ImageFormat.Png);

			BitmapImage image = new BitmapImage();
			image.BeginInit();
			image.CacheOption = BitmapCacheOption.OnLoad;
			stream.Seek(0, SeekOrigin.Begin);
			image.StreamSource = stream;
			image.EndInit();

			return image;
		}

		//  Programs
		private static string TryGetRegisteredApplication(string extension)
		{
			string ext = GetClassesRootKeyDefaultValue(extension);
			if (ext == null)
				return null;

			string openCommand = GetClassesRootKeyDefaultValue(Path.Combine(new[] { ext, "shell", "open", "command" }));

			if (openCommand == null)
				return null;

			return openCommand.Replace("%1", string.Empty).Replace("\"", string.Empty).Trim();
		}

		private static string GetClassesRootKeyDefaultValue(string keyPath)
		{
			using (var key = Registry.ClassesRoot.OpenSubKey(keyPath))
			{
				if (key == null)
				{
					return null;
				}

				var defaultValue = key.GetValue(null);
				if (defaultValue == null)
				{
					return null;
				}

				return defaultValue.ToString();
			}
		}

		public static void Execute(string path, string operation = "open")
		{
			Win32.ShellExecute(0, operation, path, "", "", SW_SHOWNORMAL);
		}

		public static void Execute(List<string> items, string operation = "open")
		{
			if (items.Count == 0)
				return;

			foreach (string item in items)
			{
				string ext = Path.GetExtension(item);
				string program = TryGetRegisteredApplication(ext);
				if (program == null)
					continue;

				StringBuilder builder = new StringBuilder();
				List<string> param = new List<string>();
				string path;

				int pos = program.IndexOf('.') + 1;
				pos = program.IndexOf(' ', pos);

				if (pos == -1)
				{
					path = program;
				}
				else
				{
					path = program.Substring(0, pos);
					string suffix = program.Substring(pos + 1);
					builder.Append("\"").Append(suffix).Append("\"");
				}

				builder.Append(" \"").Append(item).Append("\"");

				IntPtr ptr = Win32.ShellExecute(0, operation, path, builder.ToString(), "", SW_SHOWNORMAL);
				if (ptr.ToInt32() < 32)
					Win32.ShellExecute(0, operation, path, "", "", SW_SHOWNORMAL);
			}

			// Determine mapping
			//Dictionary<string, List<string>> map = new Dictionary<string, List<string>>();

			//foreach (string item in items)
			//{
			//	string ext = Path.GetExtension(item);
			//	string program = TryGetRegisteredApplication(ext);
			//	if (program == null)
			//		continue;

			//	List<string> param;
			//	if (!map.TryGetValue(program, out param))
			//	{
			//		param = new List<string>();
			//		map[program] = param;
			//	}

			//	param.Add(item);
			//}

			//// Execute each unique 
			//foreach (string program in map.Keys)
			//{
			//	if (program == null)
			//		continue;

			//	StringBuilder builder = new StringBuilder();
			//	List<string> param = map[program];				
			//	string path;
				
			//	int pos = program.IndexOf('.') + 1;
			//	pos = program.IndexOf(' ', pos);

			//	if (pos == -1)
			//	{
			//		path = program;
			//	}
			//	else
			//	{
			//		path = program.Substring(0, pos);
			//		string suffix = program.Substring(pos + 1);
			//		builder.Append("\"").Append(suffix).Append("\"");
			//	}

			//	foreach (string item in param)
			//		builder.Append(" \"").Append(item).Append("\"");

			//	IntPtr ptr = Win32.ShellExecute(0, operation, path, builder.ToString(), "", SW_SHOWNORMAL);
			//	if (ptr.ToInt32() < 32)
			//		Win32.ShellExecute(0, operation, path, "", "", SW_SHOWNORMAL);
			//}
		}

		// Folders
		public static string GetExecutableFolder()
		{
			return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
		}

		public static string GetDownloadFolder()
		{
			return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Downloads");
		}
	}
}
