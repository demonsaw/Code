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

namespace DemonSaw.Http
{
	using System;
	using System.Collections.Generic;
	using System.IO;
	using System.Net;
	using System.Text;

	// Delegates
	public delegate void HttpCommandHandler(HttpRequest httpRequest, HttpResponse httpResponse, object objRequest = null, object objResponse =  null);

	public abstract class HttpMessage
	{
		// Constants
		public const string EndOfLine = "\r\n";
		public const string EndOfMessage = "\r\n\r\n";
		public const int BufferSize = 1 << 10;

		// Variables
		private string _data;

		// Properties
		public string Version { get; set; }
		public string Title { get; set; }
		public abstract string Message { get; }

		protected Dictionary<string, string> Headers { get; private set; }
		protected Dictionary<string, string> Cookies { get; private set; }		

		// Properties - Helper
		public string Data
		{
			get { return _data; } 
			set
			{
				_data = value;

				if (value != null)
					ContentLength = _data.Length;
				else
					SetHeader(HttpTag.ContentLength, null);
			}
		}

		public string Session
		{
			get { return GetCookie(HttpTag.Session); }
			set { SetCookie(HttpTag.Session, value); }
		}

		public string Socket
		{
			get { return GetHeader(HttpTag.Socket); }
			set { SetHeader(HttpTag.Socket, value); }
		}

		public int ContentLength
		{
			get
			{
				string value = GetHeader(HttpTag.ContentLength);
				return (value != null) ? int.Parse(value) : 0; 
			}
			set { SetHeader(HttpTag.ContentLength, value.ToString()); }
		}

		// Constructors
		protected HttpMessage()
		{
			Headers = new Dictionary<string, string>();
			Cookies = new Dictionary<string, string>();

			// Defaults
			Version = "HTTP/1.1";
			Socket = HttpTag.KeepAlive;
		}

		protected HttpMessage(string sessionId) : this() { Session = sessionId; }

		// Interface
		public virtual void Clear()
		{
			_data = null;
			Title = null;            

			Headers.Clear();
			Cookies.Clear();

			// Defaults
			Version = "HTTP/1.1";
			Socket = HttpTag.KeepAlive;
		}

		public abstract int Parse(string value);

		// Utility
		protected void ParseHeaders(StringReader reader, string cookieName)
		{
			string line = null;
			StringBuilder sb = new StringBuilder(BufferSize);
			sb.Append(EndOfLine);

			// Host: www.example.com			
			while ((line = reader.ReadLine()) != null)
			{
				sb.Append(line).Append(EndOfLine);
				if (line.Length == 0)
					break;

				int headerPos = line.IndexOf(':');
				string headerName = line.Substring(0, headerPos).Trim();

				// Cookies
				if (cookieName != null && cookieName.Equals(headerName, StringComparison.OrdinalIgnoreCase))
				{
					char[] delimiters = { ' ', ';' };
					string cookieValue = line.Substring(headerPos + 1).Trim();
					string[] items = cookieValue.Split(delimiters, StringSplitOptions.RemoveEmptyEntries);

					// Cookie: name=value; name2=value2
					foreach (string item in items)
					{
						int pos = item.IndexOf('=');
						string name = item.Substring(0, pos).Trim();
						string value = item.Substring(pos + 1).Trim();
						SetCookie(name, WebUtility.HtmlDecode(value));
					}

					continue;
				}

				string headerValue = line.Substring(headerPos + 1).Trim();
				SetHeader(headerName, headerValue);
			}
		}

		// Session
		public bool HasSession() { return Session != null; }

		// Headers
		public void SetHeader(string key, string value)
		{
			if (!string.IsNullOrEmpty(value))
				Headers[key] = value;
			else
				Headers.Remove(key);
		}

		public string GetHeader(string key)
		{
			string value = null;
			Headers.TryGetValue(key, out value);
			return value;
		}

		// Cookies
		public void SetCookie(string key, string value)
		{
			if (!string.IsNullOrEmpty(value))
				Cookies[key] = value;
			else
				Cookies.Remove(key);
		}

		public string GetCookie(string key)
		{
			string value = null;
			Cookies.TryGetValue(key, out value);
			return value;
		}
	}
}
