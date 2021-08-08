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

#ifndef _EJA_VALUE_H_
#define _EJA_VALUE_H_

#include "system/type.h"

namespace eja
{
	class version_component;

	enum class entropy_type;
	enum class network_mode;
	enum class network_trust;

	namespace browse
	{
		extern const size_t min_rows;
		extern const size_t max_rows;
	}

	namespace chat
	{
		extern const size_t max_rows;
		extern const size_t max_text;
		extern const size_t min_text;
	}

	namespace client
	{
		extern const char* name;
		extern const char* timestamp;

		extern const size_t min_ring;
		extern const size_t max_ring;
		extern const size_t num_ring;
	}

	namespace color
	{
		extern const u32 black;
		extern const u32 blue;
		extern const u32 gray;
		extern const u32 green;
		extern const u32 orange;
		extern const u32 red;
		extern const u32 yellow;
		extern const u32 white;
	}

	namespace download
	{
		extern const size_t pending;
		extern const size_t timeout;

		extern const size_t min_throttle_size;
		extern const size_t max_throttle_size;
		extern const size_t num_throttle_size;

		extern const size_t max_retry;
	}

	namespace io
	{
		extern const bool remove;
		extern const char* hash;
		extern const size_t stack_size;

		extern const size_t max_id_size;
		extern const size_t max_name_size;
		extern const size_t max_color_size;

		extern const size_t max_path_size;
		extern const size_t max_address_size;
		extern const size_t max_port_size;

		extern const size_t max_cipher_size;
		extern const size_t max_hash_size;
		extern const size_t max_key_size;
		extern const size_t max_passphrase_size;
		extern const size_t max_salt_size;
	}

	namespace group
	{
		extern const double percent;
		extern const entropy_type type;

		extern const char* cipher;
		extern const size_t key_size;

		extern const char* hash;
		extern const size_t iterations;
	}

	namespace milliseconds
	{
		extern const size_t error;
		extern const size_t socket;
		extern const size_t timeout;
		extern const size_t tunnel;
		extern const size_t monitor;
		extern const size_t yield;

		extern const size_t ping;
		extern const size_t download;
		extern const size_t upload;

		extern const size_t file;
		extern const size_t open;
		extern const size_t progress;
		extern const size_t sync;

		extern const size_t quit;
		extern const size_t restart;
		extern const size_t retry;
		extern const size_t statusbar;
	}

	namespace network
	{
		extern const size_t min_buffer_size;
		extern const size_t max_buffer_size;
		extern const size_t num_buffer_size;

		extern const size_t min_chunk_size;
		extern const size_t max_chunk_size;
		extern const size_t num_chunk_size;
	}

	namespace pm
	{
		extern const size_t max_text;
		extern const size_t min_text;
	}

	namespace prime
	{
		extern const size_t size;
		extern const size_t buffer;
		extern const size_t reuse;
		extern const size_t threshold;
	}

	namespace router
	{
		extern const char* name;
		extern const char* address;
		extern const char* password;
		extern const u16 port;

		extern const size_t min_ring;
		extern const size_t max_ring;
		extern const size_t num_ring;

		extern const bool message;
		extern const bool transfer;
		extern const bool proxy;
		extern const bool open;
	}

	namespace search
	{
		extern const char* delimiters;

		extern const size_t min_rows;
		extern const size_t max_rows;

		extern const size_t max_text;
		extern const size_t min_text;
	}

	namespace session
	{
		extern const char* algorithm;
		extern const size_t prime_size;

		extern const char* cipher;
		extern const size_t key_size;

		extern const char* hash;
		extern const size_t iterations;
	}

	namespace share
	{
		extern const size_t min_text;
	}

	namespace software
	{
		extern const char* name;
		extern const char* author;
		extern const char* motto;

		extern const char* version;
		extern const char* build;
		extern const char* date;

		extern const char* title;
		extern const char* titlebar;
		extern const char* website;
		extern const char* help;

		extern const char* config;
		extern const char* font_awesome;
		extern const char* theme;

		extern const u32 min_version;
		extern const u32 max_version;
		extern const u32 num_version;
	}

	namespace suffix
	{
		extern const char* troll;
	}

	namespace transfer
	{
		extern const size_t queue;
		extern const size_t wait;
	}

	namespace ui
	{
		extern const size_t errors;
		extern const size_t enter;

		extern const size_t min_font_size;
		extern const size_t max_font_size;
		extern const size_t num_font_size;

		extern const size_t min_tabs;
		extern const size_t max_tabs;
		extern const size_t num_tabs;

		extern const bool browse;
		extern const bool chat;
		extern const bool client;
		extern const bool download;
		extern const bool error;
		extern const bool group;
		extern const bool help;
		extern const bool message;
		extern const bool option;
		extern const bool queue;
		extern const bool search;
		extern const bool session;
		extern const bool share;
		extern const bool status;
		extern const bool transfer;
		extern const bool upload;
	}

	namespace upload
	{
		extern const size_t pending;
		extern const size_t timeout;

		extern const size_t min_throttle_size;
		extern const size_t max_throttle_size;
		extern const size_t num_throttle_size;
	}
}

#endif
