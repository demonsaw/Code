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

#include "default_value.h"
#include "object/group/group_security.h"

namespace eja
{
	namespace default_value
	{
		const bool enabled = false;
		const char* title = "demonsaw 2.7.2";
		const char* titlebar = "demonsaw 2.7.2 [*]";
		const char* date = "May 29th, 2016";
		const char* website = "https://www.demonsaw.com";
		const char* style = "demonsaw.css";
		const char* font_awesome = "FontAwesome";
	}

	namespace default_version
	{
		const char* app = "2.7.2";
		const size_t major = 2;
		const size_t minor = 7;
		const size_t patch = 2;

		const size_t xml = 2;
		const size_t json = 2;		
	}

	namespace default_error
	{
		const char* unknown = "Unknown error";

		// Min/Max
		const size_t min_errors = 1;
		const size_t num_errors = 128;
		const size_t max_errors = 1024;
	}

	namespace default_network
	{
		const char* empty = "127.0.0.1"; 
		const char* address = "bucket1.demonbucket.com";
		const u16 port = 80;
	}

	namespace default_security
	{
		const size_t id = 16;
		const char* signature = "0123456789ABCDEF";
		const char* passphrase = "";
		const size_t nonce = 0;

		// Cipher
		const char* path = "";
		const size_t cipher_type = 0;
		const double entropy = 100.0;

		// Diffie-Hellman
		const size_t message_key_size = 128;
		const size_t message_prime_size = 128;
		const size_t transfer_key_size = 128;
		const size_t transfer_prime_size = 128;
		const size_t min_prime_size = 128;

		// Hash
		const size_t key_size = 256;
		const size_t hash_type = 0;
		const char* salt = "";
		const char* key = "";
		const size_t iterations = 1;

		// Group		
		const byte seed[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
		const size_t seed_size = sizeof(seed);
	}

	namespace default_group
	{
		const char* name = "Default Group";
		const size_t hash_type = static_cast<size_t>(group_hash_type::sha256);
	}

	namespace default_file
	{
		const char* config = "demonsaw.xml";
		const char* extension = ".temp";
	}

	namespace default_chunk
	{
		// Min/Max
		const size_t min_size = (1 << 10);
		const size_t num_size = (768 << 10);
		const size_t max_size = (32 << 20);

		const size_t min_buffer = (min_size * 16);
		const size_t num_buffer = (num_size * 16);
		const size_t max_buffer = (max_size * 16);

		const size_t min_drift = 0;
		const size_t num_drift = 0;
		const size_t max_drift = max_size;
	}

	namespace default_timeout
	{
		// General
		const size_t none = 0;
		const size_t yield = 1;
		const size_t sleep = 100;

		const size_t idle = 500;
		const size_t error = sec_to_ms(3);
		const size_t watchdog = sec_to_ms(15);

		namespace client
		{
			const size_t enabled = sec_to_ms(1);
			const size_t shutdown = sec_to_ms(3);
			const size_t idle = min_to_ms(1);
			const size_t splash = 250;

			const size_t file = 500;
			const size_t refresh = 500;

			const size_t share = sec_to_ms(1);
			const size_t transfer = sec_to_ms(1);
			const size_t chat = 50;

			// Min/Max
			const size_t min_download = sec_to_ms(15);
			const size_t num_download = sec_to_ms(60);
			const size_t max_download = sec_to_ms(600);

			const size_t min_upload = sec_to_ms(15);
			const size_t num_upload = sec_to_ms(60);
			const size_t max_upload = sec_to_ms(600);

			const size_t min_ping = sec_to_ms(15);
			const size_t num_ping = sec_to_ms(45);
			const size_t max_ping = sec_to_ms(60);
		}

		namespace router
		{
			const size_t enabled = sec_to_ms(1);
			const size_t idle = min_to_ms(1);
			const size_t download = sec_to_ms(1);
			const size_t upload = sec_to_ms(1);

			// Min/Max
			const size_t min_client = min_to_ms(3);
			const size_t num_client = min_to_ms(5);
			const size_t max_client = min_to_ms(15);

			const size_t min_queue = min_to_ms(3);
			const size_t num_queue = min_to_ms(30);
			const size_t max_queue = min_to_ms(60);

			const size_t min_spam = min_to_ms(1);
			const size_t num_spam = min_to_ms(5);
			const size_t max_spam = min_to_ms(60);

			const size_t min_transfer = min_to_ms(3);
			const size_t num_transfer = min_to_ms(5);
			const size_t max_transfer = min_to_ms(15);
		}

		namespace server
		{
			const size_t idle = min_to_ms(1);
		}
	}

	namespace default_chat
	{
		const bool audio = false;
		const bool visual = false;
		const bool muted = false;
		const char* sound = "";
		const size_t timeout = sec_to_ms(1);
		const char* timestamp = "%I:%M:%S %p";
		const u32 color = 0xFF00D087;		

		// Min/Max
		const size_t min_volume = 0;
		const size_t num_volume = 50;
		const size_t max_volume = 100;

		const size_t min_history = 1;
		const size_t num_history = (2 << 10);
		const size_t max_history = (4 << 10);

		const size_t min_text = 1;
		const size_t max_text = (1 << 10);
	}

	namespace default_message
	{
		const bool audio = false;
		const bool visual = false;
		const char* sound = "";
		const size_t timeout = sec_to_ms(1);
		const char* timestamp = "%I:%M:%S %p";
		const u32 color = 0xFF00D087;		

		// Min/Max
		const size_t min_volume = 0;
		const size_t num_volume = 50;
		const size_t max_volume = 100;

		const size_t min_text = 1;
		const size_t max_text = (1 << 10);
	}

	namespace default_search
	{
		const size_t timeout = sec_to_ms(1);

		// Min/Max
		const size_t min_keywords = 3;
		const size_t max_keywords = 64;
	}

	namespace default_socket
	{
		// Min/Max
		const size_t min_timeout = sec_to_ms(3);
		const size_t num_timeout = sec_to_ms(15);
		const size_t max_timeout = sec_to_ms(60);

		const bool keep_alive = true;
	}

	namespace default_client
	{
		const char* name = "(Client)";
		const bool partial = true;

		// Min/Max
		const size_t max_entities = 16; 
		
		const size_t min_downloads = 1;
		const size_t num_downloads = 4;
		const size_t max_downloads = 16;

		const size_t min_download_threads = 1;
		const size_t num_download_threads = 4;
		const size_t max_download_threads = 8;

		const size_t min_uploads = 1;
		const size_t num_uploads = 4;
		const size_t max_uploads = 16;

		const size_t min_upload_threads = 1;
		const size_t num_upload_threads = 4;
		const size_t max_upload_threads = 8;

		const size_t min_thread_size = 0;
		const size_t num_thread_size = (4 << 20);
		const size_t max_thread_size = (128 << 20);

		const size_t min_priority_size = 0;
		const size_t num_priority_size = 0;
		const size_t max_priority_size = (128 << 20);

		const size_t min_retries = 0;
		const size_t num_retries = 3;
		const size_t max_retries = 16;

		const size_t min_searches = 0;
		const size_t num_searches = 128;
		const size_t max_searches = (1 << 10);

		const size_t max_id = 32;
		const size_t max_name = 32;

		namespace action
		{
			const bool browse = true; 
			const bool search = true;
			const bool transfer = true;
			const bool chat = true;
			const bool message = true;
			const bool motd = true;
		}
	}

	namespace default_router
	{
		const char* name = "(Router)";
		const size_t thread_pool = 128;

		// Min/Max
		const size_t max_entities = 8;

		const size_t min_threads = 1;
		const size_t num_threads = 32;
		const size_t max_threads = 128;

		const size_t min_transfers = 1;
		const size_t num_transfers = (1 << 10);
		const size_t max_transfers = (4 << 10);

		namespace action
		{
			const bool message = true;
			const bool public_group = true;
			const bool transfer = true;
		}
	}

	namespace default_view
	{
		const bool option = true;
		const bool toolbar = true;
		const bool menubar = true;
		const bool statusbar = true;
		const bool detail = true;
	}
}
