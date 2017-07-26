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

#ifndef _EJA_JSON_
#define _EJA_JSON_

namespace eja
{
	namespace json
	{
		// Packet
		static const char* version = "Version";
		static const char* nonce = "Nonce";		
		static const char* session = "Session";		
		static const char* data = "Data";
		
		// Message
		static const char* message = "Message";
		static const char* type = "Type";
		static const char* action = "Action";

		// General
		static const char* id = "Id";
		static const char* name = "Name";

		static const char* passphrase = "Passphrase";
		static const char* address = "Address";
		static const char* port = "Port";

		// Entity				
		static const char* entities = "Entities"; 
		static const char* entity = "Entity";		

		// Client
		static const char* clients = "Clients";
		static const char* client = "Client";

		// Router
		static const char* routers = "Routers";
		static const char* router = "Router";

		// Server
		static const char* servers = "Servers";
		static const char* server = "Server";

		// Group
		static const char* groups = "Groups";
		static const char* group = "Group";

		// Option
		static const char* options = "Options";
		static const char* option = "Option";

		// Session
		static const char* sessions = "Sessions";

		// TODO: Remove the extra key level
		//
		// Handshake
		static const char* key = "Key";
		static const char* base = "Base";
		static const char* prime = "Prime";
		static const char* public_key = "Public";

		// Info
		static const char* info = "Info";

		// Type
		static const char* type_browse = "b";
		static const char* type_search = "s";
		static const char* type_transfer = "t";
		static const char* type_chat = "c";
		static const char* type_message = "m";

		// Search
		static const char* search = "Search";
		static const char* keyword = "Keyword";
		static const char* filter = "Filter";

		// Transfer
		static const char* files = "Files"; 
		static const char* file = "File";
		
		static const char* folders = "Folders";
		static const char* folder = "Folder";
		
		static const char* size = "Size";
		static const char* chunk = "Chunk";
		static const char* delay = "Delay";
		static const char* offset = "Id";

		// Chat
		static const char* chat = "chat";
		static const char* text = "text";
	}
}

#endif
