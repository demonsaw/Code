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

#ifndef _EJA_DEFAULT_VALUE_
#define _EJA_DEFAULT_VALUE_

#include "security/hash.h"
#include "system/type.h"

namespace eja
{
	namespace default_value
	{
		extern const bool enabled;
		extern const char* title;
		extern const char* titlebar;
		extern const char* date;
		extern const char* website;
		extern const char* style;
		extern const char* font_awesome;
	}

	namespace default_version
	{
		extern const char* app;
		extern const size_t major;
		extern const size_t minor;
		extern const size_t patch;

		extern const size_t xml;
		extern const size_t json;		
	}

	namespace default_error
	{
		extern const char* unknown;

		// Min/Max			
		extern const size_t min_errors;
		extern const size_t num_errors;
		extern const size_t max_errors;
	}

	namespace default_network
	{		
		extern const char* empty;
		extern const char* address;
		extern const u16 port;
	}

	namespace default_security
	{
		extern const size_t id;
		extern const char* signature;
		extern const char* passphrase;
		extern const size_t nonce;

		// Cipher
		extern const char* path;
		extern const size_t cipher_type;
		extern const double entropy;

		// Diffie-Hellman
		extern const size_t message_key_size;
		extern const size_t message_prime_size;
		extern const size_t transfer_key_size;
		extern const size_t transfer_prime_size;
		extern const size_t min_prime_size;

		// Hash
		extern const size_t key_size; 
		extern const size_t hash_type;
		extern const char* salt;
		extern const char* key;
		extern const size_t iterations;

		// Group		
		extern const byte seed[];
		extern const size_t seed_size;
	}

	namespace default_group
	{
		extern const char* name;
		extern const size_t hash_type;
	}

	namespace default_file
	{
		extern const char* config;
		extern const char* extension;
	}

	namespace default_chunk
	{
		// KB
		extern const size_t min_size;
		extern const size_t num_size;
		extern const size_t max_size;

		extern const size_t min_buffer;
		extern const size_t num_buffer;
		extern const size_t max_buffer;

		extern const size_t min_drift;
		extern const size_t num_drift;
		extern const size_t max_drift;
	}

	namespace default_timeout
	{
		// General
		extern const size_t none;
		extern const size_t yield;
		extern const size_t sleep;

		extern const size_t idle;
		extern const size_t error;
		extern const size_t watchdog;

		namespace client
		{
			extern const size_t enabled;
			extern const size_t shutdown;
			extern const size_t idle;
			extern const size_t splash;

			extern const size_t file;
			extern const size_t refresh;

			extern const size_t share;
			extern const size_t transfer;
			extern const size_t chat;

			// Min/Max			
			extern const size_t min_download;
			extern const size_t num_download;
			extern const size_t max_download;

			extern const size_t min_upload;
			extern const size_t num_upload;
			extern const size_t max_upload;

			extern const size_t min_ping;
			extern const size_t num_ping;
			extern const size_t max_ping;
		}

		namespace router
		{
			extern const size_t enabled;
			extern const size_t idle;
			extern const size_t download;
			extern const size_t upload;

			// Min/Max
			extern const size_t min_client;
			extern const size_t num_client;
			extern const size_t max_client;

			extern const size_t min_queue;
			extern const size_t num_queue;
			extern const size_t max_queue;

			extern const size_t min_spam;
			extern const size_t num_spam;
			extern const size_t max_spam;

			extern const size_t min_transfer;
			extern const size_t num_transfer;
			extern const size_t max_transfer;
		}

		namespace server
		{
			extern const size_t idle;
		}
	}

	namespace default_chat
	{
		extern const bool audio;
		extern const bool visual;
		extern const bool muted;
		extern const char* sound;
		extern const size_t timeout;
		extern const char* timestamp;
		extern const u32 color;
		
		// Min/Max
		extern const size_t min_volume;
		extern const size_t num_volume;
		extern const size_t max_volume;

		extern const size_t min_history;
		extern const size_t num_history;
		extern const size_t max_history;

		extern const size_t min_text;
		extern const size_t max_text;
	}

	namespace default_message
	{
		extern const bool audio;
		extern const bool visual;
		extern const char* sound;
		extern const size_t timeout;
		extern const char* timestamp;
		extern const u32 color;

		// Min/Max
		extern const size_t min_volume;
		extern const size_t num_volume;
		extern const size_t max_volume;

		extern const size_t min_text;
		extern const size_t max_text;
	}

	namespace default_search
	{
		extern const size_t timeout;

		// Min/Max
		extern const size_t min_keywords;
		extern const size_t max_keywords;
	}

	namespace default_socket
	{
		// Min/Max
		extern const size_t min_timeout;
		extern const size_t num_timeout;
		extern const size_t max_timeout;

		extern const bool keep_alive;
	}

	namespace default_client
	{
		extern const char* name;
		extern const bool partial;		

		// Min/Max
		extern const size_t max_entities;

		extern const size_t min_downloads;
		extern const size_t num_downloads;
		extern const size_t max_downloads;

		extern const size_t min_download_threads;
		extern const size_t num_download_threads;
		extern const size_t max_download_threads;

		extern const size_t min_uploads;
		extern const size_t num_uploads;
		extern const size_t max_uploads;

		extern const size_t min_upload_threads;
		extern const size_t num_upload_threads;
		extern const size_t max_upload_threads;

		extern const size_t min_thread_size;
		extern const size_t num_thread_size;
		extern const size_t max_thread_size;

		extern const size_t min_priority_size;
		extern const size_t num_priority_size;
		extern const size_t max_priority_size;

		extern const size_t min_retries;
		extern const size_t num_retries;
		extern const size_t max_retries;

		extern const size_t min_searches;
		extern const size_t num_searches;
		extern const size_t max_searches;

		extern const size_t max_id;
		extern const size_t max_name;

		namespace action
		{
			extern const bool search;
			extern const bool browse;
			extern const bool transfer;
			extern const bool chat;
			extern const bool message;
			extern const bool motd;
		}
	}

	namespace default_router
	{
		extern const char* name;
		extern const size_t thread_pool;

		// Min/Max
		extern const size_t max_entities;

		extern const size_t min_threads;
		extern const size_t num_threads;
		extern const size_t max_threads;

		extern const size_t min_transfers;
		extern const size_t num_transfers;
		extern const size_t max_transfers;

		namespace action
		{
			extern const bool message;
			extern const bool public_group;
			extern const bool transfer;
		}
	}

	namespace default_view
	{
		extern const bool option;
		extern const bool toolbar;
		extern const bool menubar;
		extern const bool statusbar;
		extern const bool detail;
	}
}

#endif
