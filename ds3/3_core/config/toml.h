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

#ifndef _EJA_TOML_H_
#define _EJA_TOML_H_

namespace eja
{
	namespace toml
	{
		// TODO: Alphabetize these?
		//
		//
		const char* demonsaw = "demonsaw";
		const char* version = "version";
		const char* theme = "theme";
		const char* index = "index";
		const char* none = "none";

		// Client
		const char* client = "client";		

		// Endpoint		
		const char* enabled = "enabled";
		const char* name = "name";
		const char* color = "color";

		// Function
		const char* browse = "browse";
		const char* chat = "chat";
		const char* pm = "pm";
		const char* group = "group";		
		const char* search = "search";		

		// IO
		const char* io = "io";
		const char* path = "path";
		const char* remove = "remove";

		const char* shares = "shares";
		const char* share = "share";
		
		const char* files = "files";
		const char* file = "file";
		
		const char* folders = "folders";
		const char* folder = "folder";

		// Option
		const char* option = "option";
		const char* beep = "beep"; 
		const char* flash = "flash";
		const char* vibrate = "vibrate";
		const char* threads = "threads";
		const char* timestamp = "timestamp";		

		// Network
		const char* mode = "mode";
		const char* network = "network";
		const char* timeout = "timeout";
		const char* address = "address";
		const char* address_ext = "address_ext";
		const char* port = "port";
		const char* port_ext = "port_ext";

		const char* transfer = "transfer";
		const char* downloads = "downloads";
		const char* download = "download";
		const char* uploads = "uploads";
		const char* upload = "upload";
		const char* queue = "queue";

		const char* chunks = "chunks";
		const char* chunk = "chunk";
		const char* retries = "retries";
		const char* retry = "retry";
		const char* shards = "shards";
		const char* shard = "shard";
		const char* windows = "windows";
		const char* window = "window";

		const char* type = "type";
		const char* trust = "trust";
		const char* interval = "interval";
		const char* sockets = "sockets";

		const char* header = "header";
		const char* redirect = "redirect";
		const char* motd = "motd";

		// Prime
		const char* prime = "prime";
		const char* size = "size";
		const char* buffer = "buffer";
		const char* reuse = "reuse";
		const char* threshold = "threshold";

		// Router
		const char* router = "router";		
		const char* message = "message";
		const char* proxy = "proxy";
		const char* open = "open";

		// Security		
		const char* security = "security"; 		
		const char* session = "session";

		const char* algorithm = "algorithm";
		const char* entropy = "entropy";
		const char* percent = "percent";
		const char* prime_size = "prime_size";

		const char* cipher = "cipher";
		const char* iv = "iv";
		const char* key_size = "key_size";

		const char* hash = "hash";
		const char* iterations = "iterations";
		const char* salt = "salt";

		// UI
		const char* ui = "ui"; 
		const char* geometry = "geometry";
		const char* state = "state";
		const char* help = "help";
		const char* error = "error";
		const char* status = "status";
	}
}

#endif
