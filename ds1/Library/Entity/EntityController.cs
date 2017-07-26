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

namespace DemonSaw.Entity
{
	using DemonSaw.Controller;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.Diagnostics;
	using System.Linq;
	using System.Windows.Controls;
	using System.Windows.Threading;

	public class EntityController : DemonController
	{
		#region Variables
		protected EntityObservableList EmptyList = new EntityObservableList();
		#endregion 

		#region Properties
		public DataGrid View { get; set; }
		public EntityObservableList List { get; protected set; }
		public Dispatcher Dispatcher { get { return View.Dispatcher; } }

		public ObservableCollection<Entity> Collection
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

		public Entity SelectedItem
		{
			get { return (Entity) View.SelectedItem; }
		}

		public List<Entity> SelectedItems
		{
			get { return View.SelectedItems.Cast<Entity>().ToList(); }
		}
		#endregion

		#region Constructors
		protected EntityController(DataGrid view)
		{
			View = view;
			List = new EntityObservableList(view.Dispatcher);
			View.ItemsSource = Collection;
		}

		protected EntityController(DataGrid view, TextBlock title) : base(title)
		{
			View = view;
			List = new EntityObservableList(view.Dispatcher);
			View.ItemsSource = Collection;
		}
		#endregion

		#region Interface
		public override void Update()
		{
			lock (List)
			{
				foreach (Entity entity in List)
					entity.Update();
			}
		}

		public override void Clear()
		{
			base.Clear();

			List.Clear();
			Update();
		}
		#endregion

		#region Utility
		public virtual void Add(Entity entity)
		{
			Debug.Assert(entity != null);

			List.Add(entity);

			Update();
			Stop();
		}

		public virtual void Add(List<Entity> list)
		{
			Debug.Assert(list != null);

			foreach (Entity entity in list)
				List.Add(entity);

			Update();
			Stop();
		}

		public virtual bool Remove(Entity entity)
		{
			Debug.Assert(entity != null);

			bool removed = List.Remove(entity);

			Update();

			return removed;
		}

		public virtual bool Remove(List<Entity> list)
		{
			Debug.Assert(list != null);

			int count = List.Count;
			foreach (Entity entity in list)
				List.Remove(entity);

			Update();

			return count != List.Count;
		}

		public virtual bool Contains(Entity entity)
		{
			Debug.Assert(entity != null);

			return List.Find(i => i.Equals(entity)) != null;
		}

		public virtual Entity Get()
		{
			return Get(0);
		}

		public virtual Entity Get(int index)
		{
			return List.Get(index);
		}

		public virtual void Select()
		{
			Select(SelectedItem);
		}

		public virtual void Select(Entity entity) { }
		#endregion
	}
}
