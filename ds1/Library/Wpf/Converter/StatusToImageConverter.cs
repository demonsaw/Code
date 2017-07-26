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

namespace DemonSaw.Wpf.Converter
{
	using DemonSaw.Utility;
	using System;
	using System.Globalization;
	using System.Windows.Data;

	public class StatusToImageConverter : IValueConverter
	{
		// Utility
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			DemonStatus data = (DemonStatus) value;
			string name = null;

			switch (data)
			{
				case DemonStatus.Info:
				{
					name = "white.png";
					break;
				}
				case DemonStatus.Success:
				{
					name = "green.png";
					break;
				}								
				case DemonStatus.Warning:
				{
					name = "yellow.png";
					break;
				}
				case DemonStatus.Error:
				{
					name = "red.png";
					break;
				}
				case DemonStatus.Cancelled:
				{
					name = "black.png";
					break;
				}
				case DemonStatus.Paused:
				{
					name = "paused.png";
					break;
				}
				default:
				{
					name = "none.png";
					break;
				}
			}

			return string.Format("/Resources/Status/{0}", name);
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}
}
