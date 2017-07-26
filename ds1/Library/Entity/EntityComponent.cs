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
	using DemonSaw.Utility;
	using System;
	using System.ComponentModel;
	using System.Diagnostics;

	// Delegates	
	public delegate void ComponentEventHandler();
	//public delegate void ComponentEventHandler(EntityComponent component);

	public abstract class EntityComponent : IState, INotifyPropertyChanged, IDisposable
	{
		// Properties
		public Entity Owner { get; set; }		

		// Events
		public event ComponentEventHandler ComponentChanged;

		// Properties - Helpers
		public virtual bool Valid { get { return true; } }
		public bool Invalid { get { return !Valid; } }

		// Handlers
		private EntityEventHandler EntityChangedHandler { get; set; }

		// Events
		public event PropertyChangedEventHandler PropertyChanged;

		// Constructors
		protected EntityComponent()
		{
			EntityChangedHandler = new EntityEventHandler(EntityChangedListener);
		}

		#region System
		void IDisposable.Dispose()
		{
			Shutdown();
		}
		
		public void OnPropertyChanged(string name)
		{
			PropertyChangedEventHandler handler = PropertyChanged;

			if (handler != null)
			{
				handler(this, new PropertyChangedEventArgs(name));
				OnComponentChanged();
			}				
		}
		#endregion

		#region Event
		protected void OnComponentChanged()
		{
			if (ComponentChanged != null)
				ComponentChanged();
		}
		#endregion

		#region Listeners
		protected virtual void EntityChangedListener() { }
		#endregion

		#region Interface
		public virtual void Init() 
		{
			Debug.Assert(Owner != null);

			Owner.EntityChanged += EntityChangedHandler;
		}

		public virtual void Update()
		{
			Debug.Assert(Owner != null);
		}

		public virtual void Shutdown()
		{
			Debug.Assert(Owner != null);

			Owner.EntityChanged -= EntityChangedHandler;
		}
		#endregion
	}
}
