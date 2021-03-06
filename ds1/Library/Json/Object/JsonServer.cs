﻿//
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

namespace DemonSaw.Json.Object
{
	using DemonSaw.Component;
	using Newtonsoft.Json;

	public sealed class JsonServer : JsonObject
	{
		#region Properties
		public string Address { get; set; }
		public int Port { get; set; }
		#endregion

		#region Constructors
		public JsonServer() { }
		public JsonServer(string id) : base(id) { }
		public JsonServer(string id, string name) : base(id, name) { }
		#endregion

		#region Operators
		public static implicit operator ServerComponent(JsonServer server) { return new ServerComponent(server.Id, server.Name); }
		#endregion

		#region Utility
		public new static JsonServer Parse(string value)
		{
			return (value != null) ? JsonConvert.DeserializeObject<JsonServer>(value) : null;
		}
		#endregion
	}
}
