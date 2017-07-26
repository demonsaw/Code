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

namespace DemonSaw.Json
{
	using Newtonsoft.Json;
	using System.IO;
	using System.Text;

	public abstract class JsonBase
	{
		// Properties - Helper
		[JsonIgnore]
		public int Length { get { return ToString().Length; } }

		// Override
		public override string ToString()
		{
			JsonSerializer serializer = new JsonSerializer();
			serializer.NullValueHandling = NullValueHandling.Ignore;

			using (StringWriter sw = new StringWriter())
			using (JsonWriter jw = new JsonTextWriter(sw))
			{
				serializer.Serialize(jw, this);
				return sw.GetStringBuilder().ToString();
			}
		}

		// Operators
		public static implicit operator string(JsonBase json) { return json.ToString(); }
		public static implicit operator byte[](JsonBase json) { return json.ToBytes(); }

		// Utility
		public byte[] ToBytes() { return Encoding.ASCII.GetBytes(ToString()); }

		// Static
		public static bool IsJson(string value) { return value != null && value.StartsWith("{"); }
		protected static JsonBase Parse(string value) { return IsJson(value) ? JsonConvert.DeserializeObject<JsonBase>(value) : null; }
		protected static T Parse<T>(string value) where T : JsonBase { return IsJson(value) ? JsonConvert.DeserializeObject<T>(value) : null; }
	}
}
