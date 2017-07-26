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

namespace DemonSaw.Wpf.TreeList
{
	using System.ComponentModel;
	using System.Windows.Controls;
	using System.Windows.Input;

	public class TreeListItem : ListViewItem, INotifyPropertyChanged
	{
		// Variables
		private TreeNode _node;

		// Properties - Helper
		public TreeNode Node
		{
			get { return _node; }
			internal set
			{
				_node = value;
				OnPropertyChanged("Node");
			}
		}

		// Handlers
		public event PropertyChangedEventHandler PropertyChanged;

		// Constructors
		public TreeListItem() { }

		// Events
		protected override void OnKeyDown(KeyEventArgs e)
		{
			if (Node != null)
			{
				switch (e.Key)
				{
					case Key.Right:
						e.Handled = true;
						if (!Node.IsExpanded)
						{
							Node.IsExpanded = true;
							ChangeFocus(Node);
						}
						else if (Node.Children.Count > 0)
							ChangeFocus(Node.Children[0]);
						break;

					case Key.Left:

						e.Handled = true;
						if (Node.IsExpanded && Node.IsExpandable)
						{
							Node.IsExpanded = false;
							ChangeFocus(Node);
						}
						else
							ChangeFocus(Node.Parent);
						break;

					case Key.Subtract:
						e.Handled = true;
						Node.IsExpanded = false;
						ChangeFocus(Node);
						break;

					case Key.Add:
						e.Handled = true;
						Node.IsExpanded = true;
						ChangeFocus(Node);
						break;
				}
			}

			if (!e.Handled)
				base.OnKeyDown(e);
		}

		private void ChangeFocus(TreeNode node)
		{
			var tree = node.Tree;
			if (tree != null)
			{
				var item = tree.ItemContainerGenerator.ContainerFromItem(node) as TreeListItem;
				if (item != null)
					item.Focus();
				else
					tree.PendingFocusNode = node;
			}
		}

		private void OnPropertyChanged(string name)
		{
			if (PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(name));
		}
	}
}