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

namespace DemonSaw.Json.Object
{
	using Newtonsoft.Json;
	using System;

	public class JsonObject : JsonBase, IComparable<JsonObject>
	{
		#region Properties
		public string Id { get; set; }
		public string Name { get; set; }
		#endregion

		#region Constructors
		public JsonObject() { }
		public JsonObject(string id) { Id = id; }
		public JsonObject(string id, string name) { Id = id; Name = name; }
		#endregion

		#region System
		#region System
		public override string ToString()
		{
			return Name;
		}

		public int CompareTo(JsonObject json)
		{
			return Name.CompareTo(json.Name);
		}

		public bool Equals(JsonObject json)
		{
			if (ReferenceEquals(null, json))
				return false;

			if (ReferenceEquals(this, json))
				return true;

			return Equals(json.Name, Name);
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
				return false;

			if (ReferenceEquals(this, obj))
				return true;

			if (obj.GetType() != typeof(JsonObject))
				return false;

			return Equals((JsonObject) obj);
		}

		public override int GetHashCode() { return Name.GetHashCode(); }
		#endregion
		#endregion

		#region Utility
		public new static JsonObject Parse(string value)
		{
			return (value != null) ? JsonConvert.DeserializeObject<JsonObject>(value) : null;
		}
		#endregion
	}
}
