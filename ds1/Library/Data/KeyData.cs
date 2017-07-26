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

namespace DemonSaw.Data
{
	using DemonSaw.Security;

	public class KeyData
	{
		// Properties
		public Key Key { get; set; }

		// Constructors
		public KeyData()
		{
			Key = new Key();
		}

		public KeyData(Key key)
		{
			Key = key;
		}

		public KeyData(byte[] data)
		{
			Key = Key.Parse(data);
		}

		public KeyData(string str)
		{
			Key = Key.Parse(str);
		}

		// Overrides
		public override string ToString() { return Key.ToString(); }
		public byte[] ToBytes() { return Key.ToBytes(); }

		// Operators		
		//public static implicit operator string(KeyData data) { return data.ToString(); }
		//public static implicit operator byte[](KeyData data) { return data.ToBytes(); }
		//public static implicit operator Key(KeyData data) { return data.Key; }

		// Interface
		public virtual void Clear()
		{
			Key.Clear();
		}

		// Utility
		public virtual string Encrypt(object obj)
		{
			if (obj == null)
				return null;

			string str = obj.ToString();
			return Key.Valid ? Key.Encrypt(str) : str;
		}

		public virtual string Decrypt(object obj)
		{
			if (obj == null)
				return null;

			string str = obj.ToString();
			return Key.Valid ? Key.Decrypt(str) : str;
		}
	}
}
