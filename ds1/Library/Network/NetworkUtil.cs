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

namespace DemonSaw.Network
{
	using DemonSaw.Utility;
	using System;
	using System.Collections.Generic;
	using System.Net;
	using System.Net.NetworkInformation;
	using System.Net.Sockets;

	public static class NetworkUtil
	{
		// Properties - Static
		public static readonly IPAddress LocalHost = new IPAddress(new byte[]{127, 0, 0, 1});

		// Utility
		public static bool IsPortValid(int value) { return value >= 1 && value <= 65535; }
		public static bool IsPortValid(string value)
		{
			if (string.IsNullOrEmpty(value))
				return false;

			int port;
			return int.TryParse(value, out port);
		}

		public static bool IsAddressValid(string value)
		{
			return IsIPValid(value) || IsURLValid(value);
		}

		public static bool IsIPValid(string value)
		{
			if (string.IsNullOrEmpty(value))
				return false;

			IPAddress ip;
			return IPAddress.TryParse(value, out ip);
		}

		public static bool IsURLValid(string value)
		{
			if (string.IsNullOrEmpty(value))
				return false;

			try 
			{
				Dns.GetHostEntry(value);
				return true;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
				return false;
			}
		}

		public static List<IPAddress> GetIPAddresses()
		{
			List<IPAddress> list = new List<IPAddress>();
			list.Add(LocalHost);

			IPHostEntry host = Dns.GetHostEntry(Dns.GetHostName());
			for (int i = 0; i < host.AddressList.Length; ++i)
			{
				if (host.AddressList[i].AddressFamily == AddressFamily.InterNetwork)
					list.Add(host.AddressList[i]);
			}

			return list;
		}

		public static IPAddress GetIPAddress()
		{
			IPHostEntry host = Dns.GetHostEntry(Dns.GetHostName());
			for (int i = 0; i < host.AddressList.Length; ++i)
			{
				if (host.AddressList[i].AddressFamily == AddressFamily.InterNetwork)
					return host.AddressList[i];
			}

			return LocalHost;
		}

		public static IPAddress GetIPAddress(string value)
		{
			try
			{
				IPAddress[] ips = Dns.GetHostAddresses(value);
				return (ips.Length > 0) ? ips[0] : null;
			}
			catch (Exception) { }

			return LocalHost;
		}

		public static bool IsPortAvailable(int port)
		{
			IPGlobalProperties properties = IPGlobalProperties.GetIPGlobalProperties();
			TcpConnectionInformation[] info = properties.GetActiveTcpConnections();

			foreach (TcpConnectionInformation tcpi in info)
			{
				if (tcpi.LocalEndPoint.Port == port)
					return false;
			}

			return true;
		}

		// Accessors
		public static string GetAddress(EndPoint endpoint) { return GetAddress(endpoint.ToString()); }
		public static string GetAddress(string endpoint)
		{
			int pos = endpoint.IndexOf(":");
			return (pos > 0) ? endpoint.Substring(0, pos) : endpoint;
		}

		public static int GetPort(EndPoint endpoint) { return GetPort(endpoint.ToString()); }
		public static int GetPort(string endpoint)
		{
			int pos = endpoint.IndexOf(":") + 1;
			return (pos > 0) ? int.Parse(endpoint.Substring(pos)) : Default.Port;
		}
	}
}
