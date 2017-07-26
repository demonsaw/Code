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

namespace DemonSaw.Security
{
	using System;
	using System.IO;
	using System.Security.Cryptography;
	using System.Text;

	public class Key
	{
		// Constant
		private static CipherMode Mode = CipherMode.CBC;

		// Properties
		public byte[] Data { get; set; }

		// Properties - Helper
		public int Length { get { return Data.Length; } }
		public bool Valid { get { return Data != null; } }
		public bool Empty { get { return Data == null; } }

		// Constructor
		public Key() { }
		public Key(Key key) { Set(key); }
		public Key(byte[] data) { Set(data); }
		public Key(string data) { Set(data); }

		// Override
		public override string ToString()
		{
			StringBuilder sb = new StringBuilder(Data.Length * 2);
			foreach (byte b in Data)
				sb.AppendFormat("{0:x2}", b);

			return sb.ToString();
		}

		public byte[] ToBytes() { return Data; }

		// Operators
		public static implicit operator string(Key key) { return key.ToString(); }
		public static implicit operator byte[](Key key) { return key.ToBytes(); }

		// Crypto
		public string Encrypt(string str)
		{
			if (string.IsNullOrEmpty(str))
				return null;

			RijndaelManaged SymmetricKey = new RijndaelManaged() { Mode = Mode };
			byte[] decrypted = Encoding.UTF8.GetBytes(str);
			byte[] encrypted = null;

			using (ICryptoTransform Encryptor = SymmetricKey.CreateEncryptor(Data, SecurityUtil.IV.Data))
			{
				using (MemoryStream MemStream = new MemoryStream())
				{
					using (CryptoStream CryptoStream = new CryptoStream(MemStream, Encryptor, CryptoStreamMode.Write))
					{
						CryptoStream.Write(decrypted, 0, decrypted.Length);
						CryptoStream.FlushFinalBlock();
						encrypted = MemStream.ToArray();
						MemStream.Close();
						CryptoStream.Close();
					}
				}
			}

			SymmetricKey.Clear();

			return Convert.ToBase64String(encrypted);
		}

		public byte[] Encrypt(byte[] decrypted)
		{
			if (decrypted == null || decrypted.Length == 0)
				return null;

			RijndaelManaged SymmetricKey = new RijndaelManaged() { Mode = Mode };
			byte[] encrypted = null;

			using (ICryptoTransform Encryptor = SymmetricKey.CreateEncryptor(Data, SecurityUtil.IV.Data))
			{
				using (MemoryStream MemStream = new MemoryStream())
				{
					using (CryptoStream CryptoStream = new CryptoStream(MemStream, Encryptor, CryptoStreamMode.Write))
					{
						CryptoStream.Write(decrypted, 0, decrypted.Length);
						CryptoStream.FlushFinalBlock();
						encrypted = MemStream.ToArray();
						MemStream.Close();
						CryptoStream.Close();
					}
				}
			}

			SymmetricKey.Clear();

			return encrypted;
		}

		public string Decrypt(string str)
		{
			if (string.IsNullOrEmpty(str))
				return null;

			RijndaelManaged SymmetricKey = new RijndaelManaged() { Mode = Mode };
			byte[] base64 = Convert.FromBase64String(str);
			byte[] decrypted = new byte[base64.Length];
			int count = 0;

			using (ICryptoTransform Decryptor = SymmetricKey.CreateDecryptor(Data, SecurityUtil.IV.Data))
			{
				using (MemoryStream MemStream = new MemoryStream(base64))
				{
					using (CryptoStream CryptoStream = new CryptoStream(MemStream, Decryptor, CryptoStreamMode.Read))
					{

						count = CryptoStream.Read(decrypted, 0, decrypted.Length);
						MemStream.Close();
						CryptoStream.Close();
					}
				}
			}

			SymmetricKey.Clear();

			return Encoding.UTF8.GetString(decrypted, 0, count);
		}

		public byte[] Decrypt(byte[] encrypted)
		{
			if (encrypted == null)
				return null;

			RijndaelManaged SymmetricKey = new RijndaelManaged() { Mode = Mode };
			byte[] decrypted = new byte[encrypted.Length];
			int count = 0;

			using (ICryptoTransform Decryptor = SymmetricKey.CreateDecryptor(Data, SecurityUtil.IV.Data))
			{
				using (MemoryStream MemStream = new MemoryStream(encrypted))
				{
					using (CryptoStream CryptoStream = new CryptoStream(MemStream, Decryptor, CryptoStreamMode.Read))
					{

						count = CryptoStream.Read(decrypted, 0, decrypted.Length);
						MemStream.Close();
						CryptoStream.Close();
					}
				}
			}

			SymmetricKey.Clear();

			Array.Resize(ref decrypted, count);
			return decrypted;
		}

		// Utility
		public void Clear()
		{
			Data = null;
		}

		public void Set(Key key)
		{
			Set(key.Data); 
		}

		public void Set(byte[] data)
		{
			Data = new byte[data.Length];
			Buffer.BlockCopy(data, 0, Data, 0, Data.Length);
		}

		public void Set(string data)
		{
			Data = Encoding.ASCII.GetBytes(data);
		}

		// Static
		public static Key Parse(byte[] data)
		{
			return new Key(data);
		}

		public static Key Parse(string data)
		{
			return new Key(data);
		}
	}
}
