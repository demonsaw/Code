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

namespace DemonSaw.Controller
{
	using DemonSaw.Collection;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Diagnostics;
	using System.Linq;
	using System.Windows.Controls;
	using System.Windows.Controls.Primitives;
	using System.Windows.Media;
	using System.Windows.Threading;

	public class GridController<T> : DemonController
	{
		#region Variables
		protected ObservableList<T> EmptyList = new ObservableList<T>();
		#endregion 

		#region Properties
		public DataGrid View { get; set; }
		public ObservableList<T> List { get; protected set; }
		public Dispatcher Dispatcher { get { return View.Dispatcher; } }

		public ObservableCollection<T> Collection 
		{ 
			get { return List.Collection; } 
		}

		public virtual int Count
		{
			get { return List.Count; }
		}

		public virtual bool Empty 
		{ 
			get { return List.Empty; } 
		}

		public bool Selected
		{
			get { return View.SelectedItem != null; }
		}

		public int SelectedIndex 
		{ 
			get { return View.SelectedIndex; } 
		}
		
		public T SelectedItem 
		{ 
			get { return (T) View.SelectedItem; } 
		}
		
		public List<T> SelectedItems 
		{ 
			get { return View.SelectedItems.Cast<T>().ToList(); } 
		}
		#endregion

		#region Constructors
		protected GridController(DataGrid view) 
		{
			View = view;
			List = new ObservableList<T>(view.Dispatcher);
			View.ItemsSource = Collection;
		}

		protected GridController(DataGrid view, TextBlock title) : base(title)
		{
			View = view;
			List = new ObservableList<T>(view.Dispatcher);
			View.ItemsSource = Collection;
		}
		#endregion

		#region Interface
		public override void Clear()
		{
			base.Clear();

			List.Clear();
			Update();
		}
		#endregion

		#region Utility
		public virtual void Add(T item)
		{
			Debug.Assert(item != null);
			
			List.Add(item);

			Update();
			Stop();
		}

		public virtual void Add(List<T> list)
		{
			Debug.Assert(list != null);

			foreach (T item in list)
				List.Add(item);

			Update();
			Stop();
		}

		public virtual bool Remove(T item)
		{
			Debug.Assert(item != null);

			bool removed = List.Remove(item);		
			
			Update();

			return removed;
		}

		public virtual bool Remove(List<T> list)
		{
			Debug.Assert(list != null);

			int count = List.Count;
			foreach (T item in list)
				List.Remove(item);

			Update();

			return count != List.Count;
		}

		public virtual bool Contains(T item)
		{
			Debug.Assert(item != null);

			return List.Find(i => i.Equals(item)) != null;
		}

		public virtual T Get() 
		{ 
			return Get(0); 
		}
		
		public virtual T Get(int index)
		{
			return List.Get(index);
		}

		public virtual void Select()
		{
			Select(SelectedItem);
		}

		public virtual void Select(T item) { }
		#endregion

		#region DataGrid
		private DataGridCellsPresenter GetVisualChild(Visual parent)
		{
			DataGridCellsPresenter child = null;

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parent); i++)
			{
				Visual visual = (Visual) VisualTreeHelper.GetChild(parent, i);
				child = (DataGridCellsPresenter) visual;

				if (child == null)
					child = GetVisualChild(visual);

				if (child != null)
					break;
			}

			return child;
		}

		public DataGridRow GetSelectedRow()
		{
			return (DataGridRow) View.ItemContainerGenerator.ContainerFromItem(View.SelectedItem);
		}

		public DataGridRow GetRow(int index)
		{
			DataGridRow row = (DataGridRow) View.ItemContainerGenerator.ContainerFromIndex(index);

			if (row == null)
			{
				// May be virtualized, bring into view and try again.
				View.UpdateLayout();
				View.ScrollIntoView(View.Items[index]);
				row = (DataGridRow) View.ItemContainerGenerator.ContainerFromIndex(index);
			}
			
			return row;
		}

		public DataGridCell GetCell(DataGridRow row, int column)
		{
			if (row != null)
			{
				DataGridCellsPresenter presenter = GetVisualChild(row);

				if (presenter == null)
				{
					View.ScrollIntoView(row, View.Columns[column]);
					presenter = GetVisualChild(row);
				}

				return (DataGridCell) presenter.ItemContainerGenerator.ContainerFromIndex(column);
			}

			return null;
		}
		#endregion	
	}
}
