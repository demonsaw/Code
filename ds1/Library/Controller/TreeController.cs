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
	using DemonSaw.Wpf.TreeList;
	using System.Collections.Generic;
	using System.Linq;
	using System.Windows.Controls;

	public class TreeController<T> : DemonController where T : ITreeModel
	{
		#region Properties
		public TreeList View { get; set; }

		public bool Selected
		{
			get { return SelectedNode != null; }
		}

		public int SelectedIndex 
		{ 
			get { return View.SelectedIndex; } 
		}
		
		public TreeNode SelectedNode 
		{ 
			get { return View.SelectedNode; } 
		}
		
		public List<TreeNode> SelectedNodes 
		{ 
			get { return View.SelectedNodes.Cast<TreeNode>().ToList(); } 
		}

		public T SelectedItem 
		{
			get { return (SelectedNode != null) ? (T) SelectedNode.Tag : default(T); } 
		}

		public List<T> SelectedItems
		{
			get
			{
				List<T> items = new List<T>();
				foreach (TreeNode node in SelectedNodes)
					items.Add((T) node.Tag);
				return items;
			}
		}
		#endregion

		#region Constructors
		public TreeController(TreeList view)
		{ 
			View = view;			
		}
		
		public TreeController(TreeList view, TextBlock title) : base(title) 
		{ 
			View = view;
		}
		#endregion

		#region Interface
		public override void Clear()
		{
			base.Clear();
						
			Update();
		}
		#endregion

		#region Utility
		public void Add(List<T> items)
		{
			foreach (T item in items)
				Add(item);
		}

		public virtual void Add(T item) { }

		public bool Remove()
		{
			return Remove(SelectedItems);
		}

		public bool Remove(List<T> items)
		{
			bool removed = true;

			foreach (T item in items)
				removed &= Remove(item);

			return removed;
		}

		public virtual bool Remove(T item) 
		{ 
			return true; 
		}

		public virtual void Select()
		{
			Select(SelectedItem);
		}

		public virtual void Select(T item)
		{
			if (item != null)
			{
				ITreeModel model = (ITreeModel) item;

				if (model.Node == null)
				{
					model.Node = new TreeNode(View, null) { IsExpanded = true };
					View.Model = model;
					View.Clear();
				}
				else
				{
					View.Model = model;
					View.Update();
				}
			}
			else
			{
				View.Model = null;
				View.Clear();
			}

			Update();
		}
		#endregion
	}
}
