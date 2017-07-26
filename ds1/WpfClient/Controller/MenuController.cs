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
	using DemonSaw.Component;
	using DemonSaw.Entity;
	using DemonSaw.Utility;

	public sealed class MenuController : DemonController
	{
		#region Variables
		private ObservableList<ClientComponent> _menu = new ObservableList<ClientComponent>();
		#endregion

		#region Properties
		public Entity Entity { get; set; }
		public ObservableList<ClientComponent> Menu { get { return _menu; } }

		public int Count { get { return Menu.Count; } }
		public bool Empty { get { return Menu.Empty; } }
		public bool Selected { get { return Entity != null; } }
		public int SelectedIndex { get { return _menu.IndexOf(SelectedItem); } }
		public ClientComponent SelectedItem { get { return (Entity != null) ? Entity.Get<ClientComponent>() : null; } }

		private static MainWindow Main { get { return Global.Main; } }
		private static SearchController SearchController { get { return Global.SearchController; } }
		private static BrowseController BrowseController { get { return Global.BrowseController; } }
		#endregion

		#region Interface
		public override void Init()
		{
			base.Init();

			Main.ClientMenu.ItemsSource = Menu.Collection; 
		}

		public override void Update()
		{
			base.Update();

			Main.ClientMenu.ItemsSource = null;
			Main.ClientMenu.ItemsSource = Menu.Collection;
			Main.ClientMenu.Header = (SelectedItem != null) ? SelectedItem.Name : Constant.None;

			SearchController.Update();
			BrowseController.Update();
		}

		public override void Clear()
		{
			base.Clear();

			Menu.Clear();
			Entity = null;
		}
		#endregion

		#region Utility
		public void Add(Entity entity)
		{
			ClientComponent client = entity.Get<ClientComponent>();
			client.ComponentChanged += Update;

			Menu.Add(client);
			Menu.Sort();

			if (Count == 1)
				Select();
		}

		public bool Remove(Entity entity)
		{
			ClientComponent client = entity.Get<ClientComponent>();
			client.ComponentChanged -= Update;

			bool removed = Menu.Remove(client);

			if (removed)
			{
				if (client == SelectedItem)
				{
					SearchController.Clear();
					BrowseController.Clear();
					Select();
				}
			}

			return removed;
		}

		public void Select()
		{
			ClientComponent client = (Menu.Count > 0) ? Menu[0] : null;
			Select(client);
		}

		public void Select(Entity entity)
		{
			if (entity == null)
				return;

			ClientComponent client = entity.Get<ClientComponent>();
			Select(client);
		}

		public void Select(ClientComponent client)
		{
			if (SelectedItem == client)
				return;

			if (SelectedItem != null)
				SelectedItem.Selected = false;

			Entity entity;
			if (client != null)
			{
				client.Selected = true;
				entity = client.Owner;
				Main.ClientMenu.Header = client.Name;
				Main.SearchKeyword.Focus();
			}
			else
			{
				entity = null;
				Main.ClientMenu.Header = Constant.None;
			}

			Entity = entity;

			SearchController.Select(entity);
			BrowseController.Select(entity);
		}
		#endregion
	}
}
