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

namespace DemonSaw.Xml
{
	public static class XmlTag
	{
		// Constants
		public const string DemonSaw = "demonsaw";		

		// Client
		public const string Clients = "clients";
		public const string Client = "client";

		public const string Folders = "folders";
		public const string Folder = "folder";

		public const string Download = "download";
		public const string Group = "group";

		public const string MaxDownloads = "max_downloads";
		public const string MaxUploads = "max_uploads";				

		// Router
		public const string Routers = "routers";
		public const string Router = "router";

		public const string Ping = "ping";
		public const string Info = "info";
		public const string Search = "search";		
		public const string Browse = "browse";

		public const string MaxTransfers = "max_transfers";
		public const string MaxChunks = "max_chunks";
		public const string ChunkSize = "chunk_size";

		// Server
		public const string Servers = "servers";
		public const string Server = "server";

		// Proxy
		public const string Proxies = "proxies";
		public const string Proxy = "proxy";

		// Misc
		public const string Enabled = "enabled"; 
		public const string Version = "version";		
		public const string Options = "options";

		public const string Id = "id";
		public const string Name = "name";
		public const string Description = "description";
		public const string Path = "path";

		public const string Address = "address";
		public const string Port = "port";
		public const string Nat = "nat";

		public const string GroupName = "group"; 
		public const string GroupImage = "image";
		public const string Passphrase = "passphrase";

		// OLD
		public const string DownloadFolder = "download";
	}
}
