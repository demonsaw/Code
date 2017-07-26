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
	using DemonSaw.Command.Request;
	using DemonSaw.Http;
	using DemonSaw.Utility;
	using System;
	using System.Threading;

	public class ServerMachineComponent : MachineComponent, IDisposable
	{
		#region System
		void IDisposable.Dispose()
		{
			ShutdownState();
		}
		#endregion

		#region Interface
		public override void Init()
		{
			base.Init();

			ServerComponent server = Owner.Get<ServerComponent>();
			if (server.Enabled && Inactive)
				Start();
		}
		#endregion

		#region Machine
		protected override void Main()
		{
			do
			{
				NextState();

				switch (State)
				{
					case DemonState.Init:
					{
						InitState();
						break;
					}
					case DemonState.Handshake:
					{
						HandshakeState();
						break;
					}
					case DemonState.Ping:
					{
						PingState();
						break;
					}
					case DemonState.Quit:
					{
						QuitState();						
						break;
					}
					case DemonState.Restart:
					{
						Sleep(DemonTimeout.Restart, DemonState.Init);
						break;
					}
					case DemonState.Shutdown:
					{
						ShutdownState();
						break;
					}
				}

				Thread.Sleep(DemonTimeout.None);
			}
			while (State != DemonState.None);
		}

		private void InitState()
		{
			try
			{
				if (Owner.Invalid)
				{
					State = DemonState.Restart;
					Status = DemonStatus.Warning;
					return;
				}

				State = DemonState.Handshake;				
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;
			}
		}

		private void ShutdownState()
		{
			State = DemonState.None;
			Status = DemonStatus.None;
		}

		private void HandshakeState()
		{
			try
			{
				// NOTE: No need to close the connection
				HandshakeRequestCommand command = new HandshakeRequestCommand(Owner);
				HttpCode code = command.Execute();
				if (code != HttpCode.Ok)
				{
					State = DemonState.Restart;
					Status = DemonStatus.Error;
					return;
				}

				State = DemonState.Ping;
				Status = DemonStatus.Info;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart;
				Status = DemonStatus.Error;				
			}
		}

		private void PingState()
		{
			try
			{
				// NOTE: No need to close the connection
				PingRequestCommand command = new PingRequestCommand(Owner);
				HttpCode code = command.Execute();

				Status = (code == HttpCode.Ok) ? DemonStatus.Success : DemonStatus.Warning;
				State = DemonState.Quit;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart; 
				Status = DemonStatus.Error;				
			}
		}

		private void QuitState()
		{
			try
			{
				// NOTE: No need to close the connection
				QuitRequestCommand command = new QuitRequestCommand(Owner);
				HttpCode code = command.Execute();
				if (code != HttpCode.Ok)
					Status = DemonStatus.Warning;

				Sleep(DemonTimeout.Ping, DemonState.Init);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				State = DemonState.Restart; 
				Status = DemonStatus.Error;				
			}
		}
		#endregion
	}
}
