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
	using DemonSaw.Wpf.TreeList;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Controls.Primitives;
	using System.Windows.Media;

	public static class WpfUtil
	{
		public static DataGridCell GetGridCell(object obj)
		{
			DependencyObject dep = (DependencyObject) obj;
			while ((dep != null) && !(dep is DataGridCell) && !(dep is DataGridColumnHeader))
				dep = VisualTreeHelper.GetParent(dep);

			return (dep is DataGridCell) ? ((DataGridCell) dep) : null;
		}

		public static DataGridRow GetGridRow(object obj)
		{
			DependencyObject dep = (DependencyObject) obj;
			while ((dep != null) && !(dep is DataGridRow) && !(dep is DataGridRowHeader))
				dep = VisualTreeHelper.GetParent(dep);

			return (dep is DataGridRow) ? ((DataGridRow) dep) : null;
		}

		public static TreeListItem GetTreeListItem(object obj)
		{
			DependencyObject dep = (DependencyObject) obj;
			while ((dep != null) && !(dep is TreeListItem))
				dep = VisualTreeHelper.GetParent(dep);

			return (dep is TreeListItem) ? ((TreeListItem) dep) : null;
		}

		public static T TryFindParent<T>(this DependencyObject child) where T : DependencyObject
		{
			//get parent item
			DependencyObject parentObject = GetParentObject(child);

			//we've reached the end of the tree
			if (parentObject == null) 
				return null;

			//check if the parent matches the type we're looking for
			T parent = parentObject as T;
			if (parent != null)
			{
				return parent;
			}
			else
			{
				//use recursion to proceed with next level
				return TryFindParent<T>(parentObject);
			}
		}

		public static DependencyObject GetParentObject(this DependencyObject child)
		{
		  if (child == null) 
			  return null;

		  //handle content elements separately
		  ContentElement contentElement = child as ContentElement;
		  if (contentElement != null)
		  {
			  DependencyObject parent = ContentOperations.GetParent(contentElement);
			  if (parent != null)
				  return parent;

			  FrameworkContentElement fce = contentElement as FrameworkContentElement;
			  return fce != null ? fce.Parent : null;
		  }

		  //also try searching for parent in framework elements (such as DockPanel, etc)
		  FrameworkElement frameworkElement = child as FrameworkElement;
		  if (frameworkElement != null)
		  {
			  DependencyObject parent = frameworkElement.Parent;
			  if (parent != null)
				  return parent;
		  }

		  //if it's not a ContentElement/FrameworkElement, rely on VisualTreeHelper
		  return VisualTreeHelper.GetParent(child);
		}

		public static T FindVisualChild<T>(DependencyObject obj) where T : DependencyObject
		{
			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(obj); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(obj, i);
				if (child != null && child is T)
					return (T) child;
				else
				{
					T childOfChild = FindVisualChild<T>(child);
					if (childOfChild != null)
						return childOfChild;
				}
			}
			return null;
		}
	}
}
