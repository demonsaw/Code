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

	// Delegates
	public delegate bool FileFormatHandler(string ext);

	public static class FileFormat
	{
		// Variables
		private static HashSet<string> _audio = new HashSet<string>(new string[] { "aac", "flac", "m4a", "mp3", "wav", "wma" } );
		private static HashSet<string> _image = new HashSet<string>(new string[] { "bmp", "gif", "ico", "jpg", "jpeg", "png", "tif", "tiff" });
		private static HashSet<string> _video = new HashSet<string>(new string[] { "avi", "mov", "mpe", "mpg", "mpeg", "mkv", "ogg", "wmv" });

		// Accessors
		public static bool IsNone(string ext) { return true; }
		public static bool IsAudio(string ext) { return _audio.Contains(ext); }
		public static bool IsImage(string ext) { return _image.Contains(ext); }
		public static bool IsVideo(string ext) { return _video.Contains(ext); }

		public static FileFormatHandler GetHandler(FileFilter filter)
		{
			switch (filter)
			{
				case FileFilter.Audio:	return FileFormat.IsAudio;
				case FileFilter.Image:	return FileFormat.IsImage;
				case FileFilter.Video:	return FileFormat.IsVideo;
				default:				return FileFormat.IsNone;
			}
		}
	};
}
