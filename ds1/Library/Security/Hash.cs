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
	using System.Text;

	public static class Hash
	{
		// MD5
		public static string MD5(byte[] input)
		{
			MD5CryptoServiceProvider provider = new MD5CryptoServiceProvider();
			byte[] output = provider.ComputeHash(input);
			return BitConverter.ToString(output).Replace("-", "").ToLower();
		}

		public static string MD5(string str)
		{
			MD5CryptoServiceProvider provider = new MD5CryptoServiceProvider();
			byte[] input = ASCIIEncoding.Default.GetBytes(str);
			byte[] output = provider.ComputeHash(input);
			return BitConverter.ToString(output).Replace("-", "").ToLower();
		}

		// SHA1
		public static string SHA1(string str)
		{
			SHA1CryptoServiceProvider provider = new SHA1CryptoServiceProvider();
			byte[] input = ASCIIEncoding.Default.GetBytes(str);
			byte[] output = provider.ComputeHash(input);
			return BitConverter.ToString(output).Replace("-", "").ToLower();
		}

		// SHA256
		public static string SHA256(string str)
		{
			SHA256CryptoServiceProvider provider = new SHA256CryptoServiceProvider();
			byte[] input = ASCIIEncoding.Default.GetBytes(str);
			byte[] output = provider.ComputeHash(input);
			return BitConverter.ToString(output).Replace("-", "").ToLower();
		}

		// SHA384
		public static string SHA384(string str)
		{
			SHA384CryptoServiceProvider provider = new SHA384CryptoServiceProvider();
			byte[] input = ASCIIEncoding.Default.GetBytes(str);
			byte[] output = provider.ComputeHash(input);
			return BitConverter.ToString(output).Replace("-", "").ToLower();
		}

		// SHA512
		public static string SHA512(string str)
		{
			SHA512CryptoServiceProvider provider = new SHA512CryptoServiceProvider();
			byte[] input = ASCIIEncoding.Default.GetBytes(str);
			byte[] output = provider.ComputeHash(input);
			return BitConverter.ToString(output).Replace("-", "").ToLower();
		}
	}
}
