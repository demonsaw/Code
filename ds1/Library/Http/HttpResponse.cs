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
	using System.IO;
	using System.Net;

	// Delegates
	public delegate void HttpResponseHandler(HttpResponse httpResponse, object obj = null);

	public class HttpResponse : HttpMessage
	{
		// Constants
		public const string HEADER_COOKIE = "Set-Cookie";

		// Properties
		public HttpCode Code { get; set; }

		// Properties - Helper
		public override string Message
		{
			get
			{
				StringWriter sw = new StringWriter();

				// Http/1.1 200 OK
				sw.Write(Version);
				sw.Write(" ");
				sw.Write((int) Code);
				sw.Write(" ");
				sw.Write(Status);
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

		public string Status { get { return HttpStatus.GetStatus(Code); } }

		public bool Ok { get { return Code == HttpCode.Ok; } }		
		public bool BadRequest { get { return Code == HttpCode.BadRequest; } }
		public bool Unauthorized { get { return Code == HttpCode.Unauthorized; } }
		public bool NotFound { get { return Code == HttpCode.NotFound; } }
		public bool InternalServerError { get { return Code == HttpCode.BadRequest; } }
		public bool Error { get { return !Ok; } }

		// Constructors
		public HttpResponse() { Code = HttpCode.Ok; }
		public HttpResponse(string sessionId) : base(sessionId) { Code = HttpCode.Ok; }
		public HttpResponse(HttpCode code, string sessionId = null) : base(sessionId) { Code = code; }

		// Overrides
		public override string ToString()
		{
			StringWriter sw = new StringWriter();
			sw.Write(Message);
			sw.Write(Data);

			return sw.ToString();
		}

		// Interface
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

			// Http/1.1 200 OK
			int lpos = 0;
			int rpos = Title.IndexOf(' ');
			Version = Title.Substring(lpos, rpos).Trim();

			lpos = rpos + 1;
			rpos = Title.IndexOf(' ', lpos);
			Code = (HttpCode) int.Parse(Title.Substring(lpos, rpos - lpos).Trim());

			lpos = rpos + 1;
			string status = Title.Substring(lpos, Title.Length - lpos).Trim();

			// Headers
			ParseHeaders(reader, HEADER_COOKIE);

			return size + EndOfMessage.Length;
		}
	}
}
