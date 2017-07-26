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

namespace DemonSaw.Machine
{
	using DemonSaw.Collection;
	using DemonSaw.Http;
	using DemonSaw.Network;
	using DemonSaw.Utility;
	using System;
	using System.Net.Sockets;
	using System.Threading;

	public abstract class DemonMachine : Demon, IDisposable
	{
		// Variables
		private DemonState _state = DemonState.None;		

		// Properties
		private Deque<DemonState> States { get; set; }
		private DemonState SleepState { get; set; }		
		protected bool Paused { get; private set; }		
		protected Timer SleepTimer { get; set; }

		// Properties - Helper
		public bool Active { get { return !IsNone(); } }
		public bool Inactive { get { return IsNone(); } }

		public DemonState State
		{
			get { return _state; }
			set
			{
				lock (States)
				{
					switch (value)
					{
						case DemonState.None:
						{
							States.PushFront(value);
							break;
						}
						default:
						{
							States.PushBack(value);
							break;
						}
					}
				}
			}
		}

		// Constructors
		protected DemonMachine()
		{
			SleepState = DemonState.None;
			States = new Deque<DemonState>();
			SleepTimer = new Timer(SleepTimeout, SleepTimer, Timeout.InfiniteTimeSpan, Timeout.InfiniteTimeSpan);
		}

		// Overrides
		void IDisposable.Dispose()
		{
			if (SleepTimer != null)
			{
				SleepTimer.Dispose();
				SleepTimer = null;
			}
		}

		// Interface
		public virtual void Start()
		{			
			try
			{
				States.Clear();
				State = DemonState.Init;
				Thread thread = new Thread(new ThreadStart(Main)) { Priority = ThreadPriority.BelowNormal, IsBackground = true };
				thread.Start();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		public virtual void Stop()
		{
			State = DemonState.Shutdown;
		}

		public virtual void Restart()
		{
			State = DemonState.Restart; 
		}

		// Pause
		public void Pause() { Paused = true; }
		public virtual void UnPause() { Paused = false; }

		// Sleep
		protected void Sleep() { Sleep(DemonTimeout.Sleep); }
		protected void Sleep(TimeSpan time) { Sleep(time, State); }
		protected void Sleep(DemonState state) { Sleep(DemonTimeout.Sleep, state); }
		protected virtual void Sleep(TimeSpan time, DemonState state) 
		{
			if (SleepState == DemonState.None)
			{
				SleepState = state;
				State = DemonState.Sleep;

				if (SleepTimer != null)
					SleepTimer.Change(time, Timeout.InfiniteTimeSpan);
			}			
		}

		private void SleepTimeout(object obj)
		{
			if (SleepState != DemonState.None)
			{
				State = SleepState;
				SleepState = DemonState.None;
			}			
		}

		// State Machine
		protected abstract void Main();

		protected void NextState()
		{
			Update();

			lock (States)
			{
				if (States.Count > 0)
					_state = States.Pop();
			}
		}

		protected virtual void Update() { }

		// TODO: Remove eventually
		//
		// Send
		protected void SendOk(NetworkConnection connection) { SendOk(connection.Socket, new HttpResponse()); }
		protected void SendOk(NetworkConnection connection, string data) { SendOk(connection.Socket, new HttpResponse() { Data = data }); }
		protected void SendOk(NetworkConnection connection, HttpResponse httpResponse) { SendOk(connection.Socket, httpResponse); }

		protected void SendOk(Socket socket) { SendOk(socket, new HttpResponse()); }
		protected void SendOk(Socket socket, string data) { SendOk(socket, new HttpResponse() { Data = data }); }
		protected void SendOk(Socket socket, HttpResponse httpResponse)
		{
			NetworkResponse netResponse = new NetworkResponse(socket);
			netResponse.Send(httpResponse);
		}

		// TODO: Remove eventually
		//
		protected void SendNotFound(NetworkConnection connection) { SendNotFound(connection.Socket, new HttpResponse(HttpCode.NotFound)); }
		protected void SendNotFound(NetworkConnection connection, string data) { SendNotFound(connection.Socket, new HttpResponse(HttpCode.NotFound) { Data = data }); }
		protected void SendNotFound(NetworkConnection connection, HttpResponse httpResponse) { SendNotFound(connection.Socket, httpResponse); }

		protected void SendNotFound(Socket socket) { SendNotFound(socket, new HttpResponse(HttpCode.NotFound)); }
		protected void SendNotFound(Socket socket, string data) { SendNotFound(socket, new HttpResponse(HttpCode.NotFound) { Data = data }); }
		protected void SendNotFound(Socket socket, HttpResponse httpResponse)
		{
			NetworkResponse netResponse = new NetworkResponse(socket);
			netResponse.Send(httpResponse);
		}

		// Accessors
		public bool IsNone() { return State == DemonState.None; }
		public bool IsConnect() { return State == DemonState.Connect; }
		public bool IsDisconnect() { return State == DemonState.Disconnect; }
		public bool IsInit() { return State == DemonState.Init; }
		public bool IsTunnel() { return State == DemonState.Tunnel; }
		public bool IsSleep() { return State == DemonState.Sleep; }
		public bool IsRestart() { return State == DemonState.Restart; }
		public bool IsListen() { return State == DemonState.Listen; }
		public bool IsShutdown() { return State == DemonState.Shutdown; }
		public bool IsPing() { return State == DemonState.Ping; }
		public bool IsHandshake() { return State == DemonState.Handshake; }
		public bool IsJoin() { return State == DemonState.Join; }
		public bool IsInfo() { return State == DemonState.Info; }
		public bool IsSearch() { return State == DemonState.Search; }
		public bool IsClient() { return State == DemonState.Client; }
		public bool IsBrowse() { return State == DemonState.Browse; }
		public bool IsDownload() { return State == DemonState.Download; }
		public bool IsUpload() { return State == DemonState.Upload; }
		public bool IsQuit() { return State == DemonState.Quit; }
	}
}
