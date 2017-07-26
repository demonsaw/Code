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

namespace DemonSaw.Wpf
{
	using System.Windows.Input;

	public static class WpfCommand
	{
		#region Menu
		public static readonly RoutedUICommand Exit = new RoutedUICommand
		(
			"Exit",
			"Exit",
			typeof(WpfCommand),
			new InputGestureCollection() { new KeyGesture(Key.F4, ModifierKeys.Alt) }
		);

		public static readonly RoutedUICommand About = new RoutedUICommand
		(
			"About",
			"About",
			typeof(WpfCommand)
		);
		#endregion

		#region Router
		public static readonly RoutedUICommand RouterAdd = new RoutedUICommand
		(
			"Router Add",
			"RouterAdd",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand RouterRemove = new RoutedUICommand
		(
			"Router Remove",
			"RouterRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand RouterEdit = new RoutedUICommand
		(
			"Router Edit",
			"RouterEdit",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand RouterUpdate = new RoutedUICommand
		(
			"Router Update",
			"RouterUpdate",
			typeof(WpfCommand)
		);
		#endregion

		#region Server
		public static readonly RoutedUICommand ServerAdd = new RoutedUICommand
		(
			"Server Add",
			"ServerAdd",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand ServerRemove = new RoutedUICommand
		(
			"Server Remove",
			"ServerRemove",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand ServerUpdate = new RoutedUICommand
		(
			"Server Update",
			"ServerUpdate",
			typeof(WpfCommand)
		);
		#endregion

		#region Log
		public static readonly RoutedUICommand LogUpdate = new RoutedUICommand
		(
			"Log Update",
			"LogUpdate",
			typeof(WpfCommand)
		);

		public static readonly RoutedUICommand LogClear = new RoutedUICommand
		(
			"Log Clear",
			"LogClear",
			typeof(WpfCommand)
		);
		#endregion
	};
}
