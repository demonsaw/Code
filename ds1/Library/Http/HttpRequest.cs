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

	// Delegates
	public delegate void HttpRequestHandler(HttpRequest httpRequest, object obj = null);

	public class HttpRequest : HttpMessage
	{
		// Constants
		public const string HEADER_COOKIE = "Cookie";

		// Properties
		public string Method { get; set; }
		public string Resource { get; set; }
		public Dictionary<string, string> QueryString { get; set; }

		// Properties - Helper
		public string Host
		{
			get { return GetHeader(HttpTag.Host); }
			set { SetHeader(HttpTag.Host, value); }
		}

		public string Accept
		{
			get { return GetHeader(HttpTag.Accept); }
			set { SetHeader(HttpTag.Accept, value); }
		}

		public override string Message
		{ 
			get
			{
				StringWriter sw = new StringWriter();

				// GET /index.html Http/1.1
				sw.Write(Method);
				sw.Write(" ");
				sw.Write(Resource);

				// QueryString - ?me=god&you=fool
				int count = QueryString.Count;
				if (count > 0)
					sw.Write("?");

				foreach (string key in QueryString.Keys)
				{
					string value = QueryString[key];
					sw.Write(key);
					sw.Write("=");
					sw.Write(value);

					if (--count > 0)
						sw.Write("&");
				}

				sw.Write(" ");
				sw.Write(Version);
				sw.Write(EndOfLine);

				// Headers
				foreach (string key in Headers.Keys)
				{
					string value = Headers[key];
					sw.Write(key);
					sw.Write(": ");
					sw.Write(value);
					sw.Write(EndOfLine);
				}

				// Cookies
				foreach (string key in Cookies.Keys)
				{
					string value = Cookies[key];
					sw.Write(HEADER_COOKIE);
					sw.Write(": ");
					sw.Write(key);
					sw.Write("=");
					sw.Write(WebUtility.HtmlEncode(value));
					sw.Write(EndOfLine);
				}

				sw.Write(EndOfLine);

				return sw.ToString();
			}
		}
		
		// Constructors
		public HttpRequest()
		{
			QueryString = new Dictionary<string, string>();

			Method = "POST";
			Resource = "/";
		}

		public HttpRequest(string sessionId) : base(sessionId)
		{
			QueryString = new Dictionary<string, string>();

			Method = "POST";
			Resource = "/";
		}

		// Overrides
		public override string ToString()
		{
			StringWriter sw = new StringWriter();
			sw.Write(Message);
			sw.Write(Data);

			return sw.ToString();
		}

		// Interface
		public override void Clear()
		{
			base.Clear();

			Method = "POST";
			Resource = "/";

			QueryString.Clear();
		}

		public override int Parse(string str)
		{
			Clear();

			int size = str.IndexOf(EndOfMessage);
			if (size == -1)
				return 0;

			// Request
			string header = str.Substring(0, size);
			StringReader reader = new StringReader(header);
			Title = reader.ReadLine();

			// GET /index.html Http/1.1
			string[] items = Title.Split(' ');
			Method = items[0].Trim();
			Resource = items[1].Trim();
			Version = items[2].Trim();

			// Query String
			int queryPos = Resource.IndexOf('?');
			if (queryPos >= 0)
			{
				string data = Resource.Substring(queryPos + 1);
				Resource = Resource.Substring(0, queryPos);
				items = data.Split('&');

				// ?me=god&you=fool
				foreach (string item in items)
				{
					int pos = item.IndexOf('=');
					string name = Uri.UnescapeDataString(item.Substring(0, pos));
					string value = Uri.UnescapeDataString(item.Substring(pos + 1));
					SetQueryString(name, value);
				}
			}

			// Headers
			ParseHeaders(reader, HEADER_COOKIE);

			return size + EndOfMessage.Length;
		}

		// QueryString
		public void SetQueryString(string key, string value)
		{
			if (!string.IsNullOrEmpty(value))
				QueryString[key] = value;
			else
				QueryString.Remove(key);
		}

		public string GetQueryString(string key)
		{
			string value = null;
			QueryString.TryGetValue(key, out value);
			return value;
		}
	}
}
