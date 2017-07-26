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
	using DemonSaw.Http;
	using DemonSaw.IO;
	using DemonSaw.Json;
	using DemonSaw.Json.Message;
	using DemonSaw.Utility;
	using System;
	using System.Text;
	using System.Windows;
	using System.Windows.Controls;

	public sealed class LogController : DemonController
	{
		// Constants
		private const string Filename = "demonsaw.log";

		// Enums
		public enum LogDevice { None, Client, Server };
		private enum Suffix { None, EOL, EOM };

		// Properties
		private TextBox View { get; set; }
		private string Path { get; set; }
		private LogDevice Device { get; set; }
		private int Count { get; set; }

		// Properties - Helper
		public bool Empty { get { return View.Text.Length <= 0; } }

		// Constructors
		public LogController(TextBox view, TextBlock title) : base(title) 
		{
			View = view;
			Path = System.IO.Path.Combine(FileUtil.GetExecutableFolder(), Filename);
		}

		#region Interface
		public override void Init()
		{
			base.Init();

			Clear();
		}

		public override void Update()
		{
			base.Update();

			try
			{
				lock (View)
				{
					byte[] data = FileUtil.Read(Path);
					View.Text = Encoding.ASCII.GetString(data);
					Title.Text = ((Count != 1) ? string.Format("{0:n0} Commands", Count) : "1 Command");
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		public override void Clear()
		{
			base.Clear();

			lock (View)
			{
				if (FileUtil.Exists(Path))
					FileUtil.Clear(Path);
				else
					FileUtil.Create(Path);
			}

			Count = 0;

			Update();
		}
		#endregion

		#region Utility
		private bool IsHandshake(string str) { return str != null && str.Length <= 16; }

		public void Add(object obj)
		{
			if (obj == null)
				return;

			StringBuilder sb = new StringBuilder();
			sb.Append(obj.ToString()).Append(Constant.Newline);
			FileUtil.Append(Path, sb.ToString());
		}

		public void Add(HttpRequest httpRequest, object objRequest = null)
		{
			lock (View)
				Add(httpRequest, objRequest, Suffix.EOL);
		}

		public void Add(HttpResponse httpResponse, object objResponse = null)
		{
			lock (View)
				Add(httpResponse, objResponse, Suffix.EOL);
		}

		public void Add(HttpRequest httpRequest, HttpResponse httpResponse, object objRequest = null, object objResponse = null)
		{
			lock (View)
			{
				++Count;
				JsonType jsonType = JsonType.Handshake;
				JsonAction jsonAction = JsonAction.Request;
				LogDevice device = LogDevice.Client;

				if (objRequest != null && objRequest is JsonPacket)
				{
					JsonPacket jsonRequest = (JsonPacket) objRequest;
					JsonMessage jsonMessage = JsonMessage.Parse(jsonRequest.Message);
					jsonAction = jsonMessage.Action;
					jsonType = jsonMessage.Type;

					if (!httpRequest.HasSession())
						device = LogDevice.Server;
				}

				StringBuilder sb = new StringBuilder();
				sb.Append("[ ").Append(device.ToString().ToUpper()).Append(" ]").Append(Constant.Newline);
				sb.Append("[ ").Append(jsonType.ToString().ToUpper()).Append(" ").Append(jsonAction.ToString().ToUpper()).Append(" ]");
				Add(sb);

				Add(httpRequest, objRequest, Suffix.EOM);
				Add(httpResponse, objResponse, Suffix.EOL);
			}
		}

		private void Add(HttpMessage http, object data, Suffix suffix = Suffix.None)
		{
			if (http == null)
				return;

			StringBuilder sb = new StringBuilder();

			if (JsonBase.IsJson(http.Data))
				data = JsonPacket.Parse(http.Data);
			else if (IsHandshake(http.Data))
				data = http.Data;

			sb.Append(http.Message.Substring(0, http.Message.Length - HttpMessage.EndOfMessage.Length));

			if (data != null)
			{
				sb.Append(HttpMessage.EndOfMessage);
				sb.Append(data);
			}

			switch (suffix)
			{
				case Suffix.EOL:
				{
					sb.Append(Constant.Newline);
					sb.Append("________________________________________________________________________________________________________________________________________________________________________________________________");					
					break;
				}
			}

			sb.Append(Constant.Newline);

			Add(sb);
		}
		#endregion
	}
}
