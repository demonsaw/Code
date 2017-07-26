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
	using System.Security.Cryptography;

	public static class SecurityUtil
	{
		// Constants
		public const string Hash = "SHA1";
		public const int Iterations = 4;
		public const int KeySize = 16;

		// Properties
		public static readonly string Token = "DemonSaw Mark5:9";
		public static readonly Key Seed = new Key(new byte[] { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF });
		public static readonly Key IV = new Key("&DgHw4%k3S8)h;p^");
		public static readonly Key Salt = new Key("69_G@ym3r5_UU======D");

		// Utility
		public static Key CreateKey(int keySize = KeySize)
		{
			Guid guid = Guid.NewGuid();
			string password = guid.ToString();

			PasswordDeriveBytes DerivedPassword = new PasswordDeriveBytes(password, Salt.Data, Hash, Iterations);
			byte[] data = DerivedPassword.GetBytes(keySize);
			return new Key(data);
		}

		public static Key CreateKey(string password, int keySize = KeySize)
		{
			PasswordDeriveBytes DerivedPassword = new PasswordDeriveBytes(password, Salt.Data, Hash, Iterations);
			byte[] data = DerivedPassword.GetBytes(keySize);
			return new Key(data);
		}

		public static string CreateKeyString(int keySize = KeySize)
		{
			return CreateKey(keySize).ToString();
		}

		public static string CreateKeyString(string password, int keySize = KeySize)
		{
			return CreateKey(password, keySize).ToString();
		}
	}
}
