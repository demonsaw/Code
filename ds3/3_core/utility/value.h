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

	enum class group_security_type;
	enum class network_mode;
	enum class network_trust;	

	namespace chat
	{
		extern const size_t min_text;
		extern const size_t max_text;
	}

	namespace client
	{
		extern const char* name;
		extern const bool beep;
		extern const bool flash;
		extern const bool vibrate;
		extern const char* timestamp;		
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

	namespace demonsaw
	{
		extern const char* id;
		extern const char* name;
		extern const u32 color;
		
		extern const char* application;
		extern const char* author;
		extern const char* copyright;
		extern const char* date;
		extern const char* motto;
		extern const char* title;
		extern const char* titlebar;
		extern const char* website;		
		
		extern const char* config;		
		extern const char* font_awesome;
		extern const char* theme;
	}

	namespace io
	{
		extern const bool remove;
		extern const char* hash;
		extern const size_t stack_size;
	}

	namespace group
	{
		extern const double percent;
		extern const group_security_type type;

		extern const char* cipher;
		extern const size_t key_size;

		extern const char* hash;
		extern const size_t iterations;
	}

	namespace milliseconds
	{
		extern const size_t socket;
		extern const size_t tunnel;
		extern const size_t yield;

		extern const size_t download;		
		extern const size_t upload;		

		extern const size_t file;
		extern const size_t open;
		extern const size_t progress;
		extern const size_t queue;

		extern const size_t sync;		
		extern const size_t timeout;		
		
		extern const size_t client;
		extern const size_t server;
		extern const size_t throttle;
		extern const size_t transfer;		
		
		extern const size_t quit;
		extern const size_t restart; 
		extern const size_t retry;
		extern const size_t status;
	}

	namespace network
	{		
		extern const size_t interval;
		extern const network_mode mode;
		extern const network_trust trust;				

		extern const bool motd;
		extern const bool version;

		extern const bool browse;
		extern const bool chat;
		extern const bool pm;
		extern const bool search;		
		extern const bool upload;

		extern const size_t min_buffer_size;
		extern const size_t max_buffer_size;
		extern const size_t num_buffer_size;

		extern const size_t min_chat;
		extern const size_t max_chat;
		extern const size_t num_chat;

		extern const size_t min_chunk_size;
		extern const size_t max_chunk_size;
		extern const size_t num_chunk_size;

		extern const size_t min_downloads;
		extern const size_t max_downloads;
		extern const size_t num_downloads;		

		extern const size_t min_pm;
		extern const size_t max_pm;
		extern const size_t num_pm;

		extern const size_t min_retries;
		extern const size_t max_retries;
		extern const size_t num_retries;

		extern const size_t min_routers;
		extern const size_t max_routers;
		extern const size_t num_routers;

		extern const size_t min_sockets;
		extern const size_t max_sockets;
		extern const size_t num_sockets;

		extern const size_t min_threads;
		extern const size_t max_threads;
		extern const size_t num_threads;

		extern const size_t min_timeout;
		extern const size_t max_timeout;
		extern const size_t num_timeout;

		extern const size_t min_uploads;
		extern const size_t max_uploads;
		extern const size_t num_uploads;

		extern const size_t min_window;
		extern const size_t max_window;
		extern const size_t num_window;
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
		extern const char* home;
		extern const char* localhost;
		extern const char* enigma;

		extern const u16 port;
		extern const u16 port_beta;

		extern const bool message;
		extern const bool transfer;
		extern const bool proxy;
		extern const bool open;
	}

	namespace search
	{
		extern const char* delimiters;
		extern const size_t min_text;
		extern const size_t max_text;
		extern const size_t max_history;
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

	namespace ui
	{
		extern const size_t errors;
		extern const size_t enter;
		extern const size_t refresh;

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

	namespace version
	{
		extern const size_t toml;
		extern const size_t msgpack;
		extern const version_component min;

		extern const char* app;
		extern const size_t major;
		extern const size_t minor;
		extern const size_t patch;
	}
}

#endif
