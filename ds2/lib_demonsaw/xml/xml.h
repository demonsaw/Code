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

#ifndef _EJA_XML_
#define _EJA_XML_

namespace eja
{
	namespace xml
	{
		// Global		
		static const char* demonsaw = "demonsaw";
		static const char* comment = "<xmlcomment>";
		static const char* attribute = "<xmlattr>";

		// Shared
		static const char* id = "id";
		static const char* name = "name";
		static const char* description = "description";
		static const char* version = "version";
		static const char* enabled = "enabled";
		static const char* confirm = "confirm";

		// Misc
		static const char* max = "max";
		static const char* min = "min";

		// View
		static const char* view = "view";
		static const char* option = "option";
		static const char* toolbar = "toolbar";
		static const char* menubar = "menubar";
		static const char* statusbar = "statusbar";
		static const char* detail = "detail";

		// Entity
		static const char* clients = "clients";
		static const char* client = "client";
		static const char* routers = "routers";
		static const char* router = "router";
		static const char* servers = "servers";
		static const char* server = "server";
		static const char* proxies = "proxies";
		static const char* proxy = "proxy";

		// Network
		static const char* redirect = "redirect"; 
		static const char* socket = "socket";
		static const char* address = "address";
		static const char* port = "port";
		static const char* nat = "nat";
		
		// Chunk
		static const char* size = "size";
		static const char* buffer = "buffer";
		static const char* drift = "drift";		

		// Message
		static const char* action = "action";
		static const char* handshake = "handshake";
		static const char* info = "info";
		static const char* quit = "quit";
		static const char* join = "join";
		static const char* tunnel = "tunnel";
		static const char* search = "search";		
		static const char* browse = "browse";		
		static const char* ping = "ping";
		static const char* chat = "chat";
		static const char* message = "message";
		static const char* download = "download";
		static const char* upload = "upload";

		// Security
		static const char* security = "security";		
		static const char* key = "key";
		static const char* key_size = "key_size";

		static const char* message_prime_size = "message_prime_size";
		static const char* message_key_size = "message_key_size";
		
		static const char* transfer_prime_size = "transfer_prime_size";
		static const char* transfer_key_size = "transfer_key_size";

		static const char* groups = "groups";
		static const char* group = "group";

		static const char* image = "image";
		static const char* passphrase = "passphrase";
		static const char* cipher_type = "cipher";
		static const char* entropy = "entropy";
		
		static const char* hash_type = "hash";
		static const char* salt = "salt";
		static const char* iterations = "iterations";

		static const char* public_group = "public_group";
		
		// IO		
		static const char* paths = "paths"; 
		static const char* path = "path";
		static const char* temp = "temp";
		static const char* files = "files";
		static const char* file = "file";
		static const char* folders = "folders";
		static const char* folder = "folder";
		static const char* extensions = "extensions";
		static const char* extension = "extension";
		static const char* share = "share";
		static const char* open = "open";

		// Transfer
		static const char* transfers = "transfers";
		static const char* transfer = "transfer";

		static const char* queues = "queues";
		static const char* queue = "queue";

		static const char* retries = "retries";
		static const char* retry = "retry";

		static const char* searches = "searches"; 
		static const char* downloads = "downloads";
		static const char* uploads = "uploads";
		static const char* partial = "partial";

		// Thread
		static const char* threads = "threads";
		static const char* thread = "thread";
		static const char* thread_pool = "thread_pool";

		// Error
		static const char* errors = "errors";
		static const char* error = "error";

		// Priority
		static const char* priority = "priority";

		// Timeout
		static const char* timeout = "timeout";
		static const char* yield = "yield";
		static const char* idle = "idle";
		static const char* sleep = "sleep";
		static const char* disabled = "disabled";
		static const char* cleanup = "cleanup";
		static const char* spam = "spam";

		// Chat
		static const char* audio = "audio"; 
		static const char* visual = "visual";		
		static const char* sound = "sound";
		static const char* timestamp = "timestamp";
		static const char* volume = "volume";
		static const char* history = "history";
	};
}

#endif
