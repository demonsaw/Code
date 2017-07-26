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

namespace DemonSaw.Utility
{
	using System;
	using System.Runtime.InteropServices;

	public static class Win32
	{
		// Offset of window style value
		public const int GWL_STYLE = -16;

		// Window style constants for scrollbars
		public const int SB_HORZ = 0;
		public const int SB_VERT = 1;
		public const int SB_BOTH = 3;
		public const int WS_VSCROLL = 0x00200000;
		public const int WS_HSCROLL = 0x00100000;

		public const int WM_CLOSE = 0x0010;
		public const int WM_USER = 0x0400;
		public const int WM_DISPLAY = WM_USER + 50;

		public const int SW_SHOWNORMAL = 1;
		public const int SW_SHOWMINIMIZED = 2;
		public const int SW_SHOWMAXIMIZED = 3;

		public const int TCM_FIRST = 0x1300;
		public const int TCM_ADJUSTRECT = (TCM_FIRST + 40);

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

		// Utility
		public static bool IsXPOrHigher()
		{
			OperatingSystem OS = Environment.OSVersion;
			return (OS.Platform == PlatformID.Win32NT) && ((OS.Version.Major > 5) || ((OS.Version.Major == 5) && (OS.Version.Minor >= 1)));
		}

		public static bool IsVistaOrHigher()
		{
			OperatingSystem OS = Environment.OSVersion;
			return (OS.Platform == PlatformID.Win32NT) && (OS.Version.Major >= 6);
		}

		#region Natives
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct SHFILEINFO
		{
			public IntPtr hIcon;
			public int iIcon;
			public int dwAttributes;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_PATH)]
			public string szDisplayName;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
			public string szTypeName;
		}

		[StructLayout(LayoutKind.Sequential)]
		public class SHELLEXECUTEINFO
		{
			public int cbSize = 60;
			public int fMask = 0;
			public int hwnd = 0;
			public string lpVerb = null;
			public string lpFile = null;
			public string lpParameters = null;
			public string lpDirectory = null;
			public int nShow = 0;
			public int hInstApp = 0;
			public int lpIDList = 0;
			public string lpClass = null;
			public int hkeyClass = 0;
			public int dwHotKey = 0;
			public int hIcon = 0;
			public int hProcess = 0;
		}

		[DllImport("user32.dll", SetLastError = true)]
		public static extern int GetWindowLong(IntPtr hWnd, int nIndex);
		
		[DllImport("user32.dll")]
		public static extern int Send(IntPtr hWnd, int wMsg, IntPtr wParam, IntPtr lParam);

		[DllImport("user32.dll")]
		public static extern int SendMessage(int hWnd, int Msg, int wParam, int lParam);

		[DllImport("user32.dll")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool SetForegroundWindow(IntPtr hWnd);

		[DllImport("user32.dll")]
		public static extern int ShowScrollBar(IntPtr hWnd, int wBar, int bShow);

		[DllImport("user32")]
		public static extern int ShowWindow(int hwnd, int nCmdShow);

		[DllImport("shell32.dll", CharSet = CharSet.Auto)]
		public static extern IntPtr ShellExecute(int hwnd, string lpOperation, string lpFile,
			string lpParameters, string lpDirectory, int nShowCmd);

		[DllImport("Shell32.dll")]
		public static extern int ShellExecuteEx(SHELLEXECUTEINFO lpExecInfo);

		[DllImport("shell32.dll", CharSet = CharSet.Auto)]
		public static extern IntPtr SHGetFileInfo(string pszPath, int dwFileAttributes,
			out SHFILEINFO psfi, int cbFileInfo, int uFlags);
		#endregion
	}
}
