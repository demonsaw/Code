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
	using System;
	using System.Threading;

	public sealed class FolderListComponent : ListComponent<FileComponent>
	{
		#region Container
		public override void Add(FileComponent file)
		{
			try
			{								
				if (Contains(file))
					return;

				base.Add(file);

				file.Type = FileType.Drive;
				FolderMapComponent folderMap = Owner.Get<FolderMapComponent>();
				folderMap.Add(file.Id, file);

				Thread thread = new Thread(new ParameterizedThreadStart(AddFiles)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
				thread.Start(file);

				Sort();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		public override bool Remove(FileComponent file)
		{
			bool removed = base.Remove(file);

			if (removed)
			{
				if (Empty)
				{
					Clear();
				}					
				else
				{
					FolderMapComponent folders = Owner.Get<FolderMapComponent>();
					folders.Remove(file.Id);

					try
					{
						Thread thread = new Thread(new ParameterizedThreadStart(RemoveFiles)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
						thread.Start(file);
					}
					catch (Exception ex)
					{
						Console.WriteLine(ex.Message);
					}
				}
			}

			return removed;
		}

		public override void Clear()
		{
			base.Clear();
			FolderMapComponent folders = Owner.Get<FolderMapComponent>();
			folders.Clear();

			FileMapComponent files = Owner.Get<FileMapComponent>();
			files.Clear();

			OnComponentChanged();
		}
		#endregion

		// TODO: Add folder size?
		//
		#region Threads
		private void AddFiles(object obj)
		{
			long size = 0;
			AddFiles(obj, ref size);
		}

		private void AddFiles(object obj, ref long size)
		{
			try
			{
				FileComponent parent = (FileComponent) obj;				

				// Folders
				FolderMapComponent folders = Owner.Get<FolderMapComponent>();

				foreach (FileComponent folder in parent.Folders)
				{
					folders.Add(folder.Id, folder);
					AddFiles(folder, ref size);
					folder.Size = size;
					size = 0;
				}

				// Files
				FileMapComponent files = Owner.Get<FileMapComponent>();

				foreach (FileComponent file in parent.Files)
				{
					files.Add(file.Id, file);
					size += file.Size;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			OnComponentChanged();
		}

		private void RemoveFiles(object obj)
		{
			try
			{
				FileComponent parent = (FileComponent) obj;

				// Folders
				FolderMapComponent folders = Owner.Get<FolderMapComponent>();

				foreach (FileComponent folder in parent.Folders)
				{
					folders.Remove(folder.Id);
					RemoveFiles(folder);
				}

				// Files
				FileMapComponent files = Owner.Get<FileMapComponent>();

				foreach (FileComponent file in parent.Files)
				{
					files.Remove(file.Id);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			OnComponentChanged();
		}
		#endregion
	}
}
