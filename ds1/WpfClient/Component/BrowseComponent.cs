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
	using DemonSaw.Command.Request;
	using DemonSaw.Utility;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Windows.Media.Imaging;

	public sealed class BrowseComponent : FileComponent
	{
		#region Properties
		public string ClientId { get; set; }
		public int Clients { get; private set; }

		public override BitmapImage Image
		{
			get
			{
				switch (Type)
				{
					case FileType.File:
					case FileType.Folder:
					{
						return base.Image;
					}
					case FileType.Drive:
					{
						return Default.ClientBitmap;
					}
					default:
					{
						return Default.ServerBitmap;
					}
				}
			}
		}
		#endregion

		#region Constructors
		public BrowseComponent() { }
		public BrowseComponent(FileComponent file) : base(file) { }
		public BrowseComponent(string path) { _path = path;  Type = FileType.Drive; }
		public BrowseComponent(string id, string path) : base(id, path) { Type = FileType.Folder; }
		public BrowseComponent(string id, string path, long size) : base(id, path, size) { Type = FileType.File; }
		#endregion

		#region Model
		public override IEnumerable GetChildren(object obj)
		{
			try
			{
				Cursor.Wait();

				if (obj != null)
				{
					BrowseComponent browse = (BrowseComponent) obj;
					return (browse.None) ? GetGroup() : GetFiles(browse);
				}

				return GetServer();
			}
			finally
			{
				Cursor.Normal();
			}
		}

		public override bool HasChildren(object obj)
		{
			BrowseComponent browse = (BrowseComponent) obj;
			return !browse.File;
		}
		#endregion

		#region Commands
		private List<BrowseComponent> GetServer()
		{
			try
			{
				BrowseComponent browse = Owner.Get<BrowseComponent>();
				List<BrowseComponent> list = new List<BrowseComponent>();
				list.Add(browse);

				return list;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			return null;
		}

		private List<BrowseComponent> GetGroup()
		{
			try
			{
				// NOTE: No need to close the connection
				GroupRequestCommand command = new GroupRequestCommand(Owner);
				List<BrowseComponent> list = command.Execute();

				Clients = list.Count;
				OnComponentChanged();
				return list;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			return null;
		}

		private List<BrowseComponent> GetFiles(BrowseComponent browse)
		{
			try
			{
				// NOTE: No need to close the connection
				BrowseRequestCommand command = new BrowseRequestCommand(Owner);
				return command.Execute(browse.ClientId, browse.Id);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			return null;
		}
		#endregion
	}
}
