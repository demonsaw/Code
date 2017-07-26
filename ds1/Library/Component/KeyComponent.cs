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
	using DemonSaw.Entity;
	using DemonSaw.Json.Object;
	using DemonSaw.Security;

	public class KeyComponent : EntityComponent
	{
		#region Properties
		public Key Key { get; set; }
		#endregion

		#region Constructors
		public KeyComponent()
		{ 
			Key = new Key(); 
		}
		
		public KeyComponent(Key key) 
		{ 
			Key = key; 
		}

		public KeyComponent(byte[] data) 
		{ 
			Key = Key.Parse(data); 
		} 

		public KeyComponent(string str) 
		{ 
			Key = Key.Parse(str); 
		}
		#endregion

		#region System
		public override string ToString() { return Key.ToString(); }
		public byte[] ToBytes() { return Key.ToBytes(); }
		#endregion

		#region Operators
		public static implicit operator Key(KeyComponent component) { return component.Key; }
		public static implicit operator string(KeyComponent component) { return component.ToString(); }
		public static implicit operator byte[](KeyComponent component) { return component.ToBytes(); }		
		#endregion

		#region Interface
		public virtual void Clear()
		{
			Key.Clear();
		}
		#endregion

		#region Utility
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
		#endregion
	}
}
