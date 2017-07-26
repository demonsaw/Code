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

namespace DemonSaw.Collection
{
	using DemonSaw.Utility;
	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Collections.ObjectModel;
	using System.ComponentModel;
	using System.Linq;
	using System.Windows.Threading;

	public class ObservableList<T> : IList<T>
	{
		// Variables
		private Dispatcher _dispatcher;
		private ObservableCollection<T> _collection;
		private List<T> _items;

		// Properties - Helper
		protected Dispatcher Dispatcher { get { return _dispatcher; } }

		// Property changed callback
		private delegate void PropertyChangedCallback(T item, PropertyChangedEventArgs e);

		// Change callbacks
		private delegate void InsertItemCallback(int index, T item);
		private delegate void RemoveAtCallback(int index);
		private delegate void SetItemCallback(int index, T item);
		private delegate void AddCallback(T item);
		private delegate void ClearCallback();
		private delegate void RemoveCallback(T item);

		/// <summary>
		/// Creates a new instance of the ObservableBackgroundList class
		/// </summary>
		/// <param name="dispatcher">The dispatcher that is used to notify the UI thread of changes</param>
		/// 	
		public ObservableList()  : this(Dispatcher.CurrentDispatcher) { }
		public ObservableList(Dispatcher dispatcher)
		{
			_dispatcher = dispatcher;
			_collection = new ObservableCollection<T>();
			_items = new List<T>();			
		}

		/// <summary>
		/// Gets the ObservableCollection that UI controls should bind to
		/// </summary>
		public ObservableCollection<T> Collection
		{
			get
			{
				if (_dispatcher.CheckAccess() == false)
					throw new InvalidOperationException("ObservableCollection only accessible from UI thread");

				return _collection;
			}
		}

		#region ICollection<T>
		bool ICollection<T>.IsReadOnly
		{
			get
			{
				return false;
			}
		}

		public bool Empty { get { return _items.Count <= 0; } }

		/// <summary>
		/// Searches for the specified object and returns the zero-based index of the
		/// first occurrence within the entire List.
		/// </summary>
		/// <param name="item">The object to locate in the List</param>
		/// <returns>The zero-based index of the first occurrence of item within the entire List
		/// if found; otherwise, –1.</returns>
		public int IndexOf(T item)
		{
			return _items.IndexOf(item);
		}

		/// <summary>
		/// Inserts an element into the List at the specified index.
		/// </summary>
		/// <param name="index">The zero-based index at which item should be inserted.</param>
		/// <param name="item">The object to insert. The value can be null for reference types.</param>
		public void Insert(int index, T item)
		{
			_items.Insert(index, item);

			_dispatcher.BeginInvoke(DispatcherPriority.Send,
				new InsertItemCallback(InsertItemFromDispatcherThread),
				index,
				new object[] { item }
			);
		}

		/// <summary>
		/// Removes the element at the specified index of the List
		/// </summary>
		/// <param name="index">The zero-based index of the element to remove.</param>
		public void RemoveAt(int index)
		{
			_items.RemoveAt(index);

			_dispatcher.BeginInvoke(DispatcherPriority.Send,
				new RemoveAtCallback(RemoveAtFromDispatcherThread),
				index
			);
		}

		/// <summary>
		/// Gets or sets the element at the specified index.
		/// </summary>
		/// <param name="index">The zero-based index of the element to get or set.</param>
		/// <returns>The element at the specified index.</returns>
		public T this[int index]
		{
			get
			{
				return _items[index];
			}
			set
			{
				_items[index] = value;
				_dispatcher.BeginInvoke(DispatcherPriority.Send,
					new SetItemCallback(SetItemFromDispatcherThread),
					index,
					new object[] { value }
				);
			}
		}

		/// <summary>
		/// Adds an object to the end of the List.
		/// </summary>
		/// <param name="item">The object to be added to the end of the List</param>
		public void Add(T item)
		{
			_items.Add(item);

			_dispatcher.BeginInvoke(DispatcherPriority.Send,
				new AddCallback(AddFromDispatcherThread),
				item
			);
		}

		/// <summary>
		/// Removes all elements from the List
		/// </summary>
		public void Clear()
		{
			_items.Clear();			

			_dispatcher.BeginInvoke(DispatcherPriority.Send,
				new ClearCallback(ClearFromDispatcherThread)
			);
		}

		/// <summary>
		/// Determines whether an element is in the List
		/// </summary>
		/// <param name="item">The object to locate in the List</param>
		/// <returns>true if item is found in the List; otherwise, false</returns>
		public bool Contains(T item)
		{
			return _items.Contains(item);
		}

		/// <summary>
		/// Copies the entire List to a compatible one-dimensional
		/// array, starting at the specified index of the target array.
		/// </summary>
		/// <param name="array">The one-dimensional System.Array that is the destination of the elements
		/// copied from System.Collections.Generic.List<T>. The System.Array must have
		/// zero-based indexing.</param>
		/// <param name="arrayIndex">The zero-based index in array at which copying begins.</param>
		public void CopyTo(T[] array, int arrayIndex)
		{
			_items.CopyTo(array, arrayIndex);
		}

		/// <summary>
		/// Gets the number of elements actually contained in the List.
		/// </summary>
		public int Count
		{
			get
			{
				return _items.Count;
			}
		}

		/// <summary>
		/// Removes the first occurrence of a specific object from the List.
		/// </summary>
		/// <param name="item">The object to remove from the List.</param>
		/// <returns>true if item is successfully removed; otherwise, false. This method also
		///     returns false if item was not found in the List</returns>
		public bool Remove(T item)
		{
			bool result = _items.Remove(item);

			//only remove the item from the UI collection if it is removed from the worker collection
			if (result == true)
			{
				_dispatcher.BeginInvoke(DispatcherPriority.Send,
					new RemoveCallback(RemoveFromDispatcherThread),
					item
				);
			}
			return result;
		}

		#endregion

		#region IEnumerable<T>

		/// <summary>
		/// Returns an enumerator that iterates through the List
		/// </summary>
		/// <returns>A System.Collections.Generic.List<T>.Enumerator for the System.Collections.Generic.List<T>.</returns>
		public IEnumerator<T> GetEnumerator()
		{
			return _items.GetEnumerator();
		}

		#endregion

		#region IEnumerable

		/// <summary>
		/// Returns an enumerator that iterates through the List
		/// </summary>
		/// <returns>Am Enumerator for the List.</returns>
		IEnumerator IEnumerable.GetEnumerator()
		{
			return _items.GetEnumerator();
		}

		#endregion

		#region Callbacks
		/// <summary>
		/// Attaches listener for the CollectionItemNotifyPropertyChanged event
		/// </summary>
		/// <param name="source">The event source</param>
		private void StartListening(T source)
		{
			ICollectionItemNotifyPropertyChanged item = source as ICollectionItemNotifyPropertyChanged;
			if (item != null)
				item.CollectionItemPropertyChanged += new PropertyChangedEventHandler(item_CollectionItemPropertyChanged);
		}

		/// <summary>
		/// Removes listener for the CollectionItemNotifyPropertyChanged event
		/// </summary>
		/// <param name="source">The event source</param>
		private void StopListening(T source)
		{
			ICollectionItemNotifyPropertyChanged item = source as ICollectionItemNotifyPropertyChanged;
			if (item != null)
				item.CollectionItemPropertyChanged -= new PropertyChangedEventHandler(item_CollectionItemPropertyChanged);
		}

		/// <summary>
		/// Handles the CollectionItemNotifyPropertyChanged event by posting to the UI thread to
		/// raise the PropertyChanged event
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void item_CollectionItemPropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_dispatcher.BeginInvoke(DispatcherPriority.Send, new PropertyChangedCallback(PropertyChangedFromDispatcherThread), sender, new object[] { e } );
		}

		/// <summary>
		/// Raises the PropertyChanged event.
		/// </summary>
		/// <param name="source"></param>
		/// <param name="e"></param>
		private void PropertyChangedFromDispatcherThread(T source, PropertyChangedEventArgs e)
		{
			ICollectionItemNotifyPropertyChanged item = source as ICollectionItemNotifyPropertyChanged;
			item.NotifyPropertyChanged(e);
		}

		//change callbacks
		private void InsertItemFromDispatcherThread(int index, T item)
		{
			_collection.Insert(index, item);
			StartListening(item);
		}

		private void RemoveAtFromDispatcherThread(int index)
		{
			StopListening(_collection[index]);
			_collection.RemoveAt(index);
		}

		private void SetItemFromDispatcherThread(int index, T item)
		{
			StopListening(_collection[index]);
			_collection[index] = item;
			StartListening(item);
		}
		private void AddFromDispatcherThread(T item)
		{
			_collection.Add(item);
			StartListening(item);
		}
		private void ClearFromDispatcherThread()
		{
			foreach (T item in _collection)
				StopListening(item);
			_collection.Clear();
		}
		private void RemoveFromDispatcherThread(T item)
		{
			StopListening(item);
			_collection.Remove(item);
		}
		#endregion

		// Utility
		public T Get(int index)
		{
			return _items[index];
		}

		public void Sort()
		{
			_items.Sort(delegate(T t1, T t2)
			{
				return t1.ToString().CompareTo(t2.ToString());
			});
		}

		public void Sort(Comparison<T> comparison)
		{
			_items.Sort(comparison);
		}

		/// <summary>
		/// Acquires an object that locks on the collection. The lock is released when the object is disposed
		/// </summary>
		/// <returns>A disposable object that unlocks the collection when disposed</returns>
		public TimedLock Lock()
		{
			return TimedLock.Lock(((ICollection) _items).SyncRoot);
		}

		public TimedLock Lock(TimeSpan timeout)
		{
			return TimedLock.Lock(((ICollection) _items).SyncRoot, timeout);
		}
	}

	public static class ObservableListExtensions
	{
		public static int Remove<T>(this ObservableList<T> list, Func<T, bool> condition)
		{
			List<T> items = list.Where(condition).ToList();
			foreach (T item in items)
				list.Remove(item);

			return items.Count;
		}

		public static T Find<T>(this ObservableList<T> list, Func<T, bool> condition)
		{
			List<T> items = list.Where(condition).ToList();
			return (items.Count > 0) ? items[0] : default(T);
		}

		public static IEnumerable<T> FindAll<T>(this ObservableList<T> list, Func<T, bool> condition)
		{
			return list.Where(condition).ToList();
		}
	}
}
