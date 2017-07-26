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
	using DemonSaw.IO;
	using System.Drawing;
	using System.Windows.Media.Imaging;

	public static class Default
	{
		// Constants
		public const int Version = 1;
		public const int Nonce = 0;

		public const string Id = "";
		public const string Name = "";
		public const string Path = "";
		public const string Group = "None";

		public const string Description = "";
		public const string Extension = "";
		
		public const byte[] Data = null;
		public const string Address = "";
		public const int Port = 80;
		public const int Size = 0;

		public const bool Enabled = true;		
		public const bool Valid = true;

		//public const string Passphrase = "Any sufficiently advanced technology is indistinguishable from magic.";
		//public const string Passphrase = "You talk of times of peace for all, and then prepare for war.";
		//public const string Passphrase = "All I have now is the emptiness that comes from being free.";
		public const string Passphrase = "";		

		public const float Progress = 0;
		public const int ClientCount = 0; 
		public const int GroupCount = 0;
		public const int Chunk = 0;

		// Static
		public static readonly Color HighlightColor = Color.FromArgb(255, 255, 128);

		public static Image NoImageEmbedded { get { return FileUtil.GetAssemblyImage("no_image.png"); } }
		public static BitmapImage NoImage { get { return FileUtil.GetResourceBitmap("no_image.png"); } }
		public static BitmapImage ServerBitmap { get { return FileUtil.GetResourceBitmap("server.png", "16"); } }
		public static BitmapImage ClientBitmap { get { return FileUtil.GetResourceBitmap("user.png", "16"); } }
		public static BitmapImage DriveBitmap { get { return FileUtil.GetResourceBitmap("drive.png", "16"); } }
	}
}
