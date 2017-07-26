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

#include <thread>

#include "component/version_component.h"
#include "component/client/client_network_component.h"
#include "component/group/group_security_component.h"
#include "security/algorithm/diffie_hellman.h"
#include "security/cipher/aes.h"
#include "security/hash/sha.h"
#include "utility/value.h"

namespace eja
{
	namespace chat
	{
		const size_t min_text = 1;
		const size_t max_text = (1 << 10);
	}

	namespace client
	{
		const char* name = "[client]";
		const bool beep = false;
		const bool flash = true;
		const bool vibrate = false;
		const char* timestamp = "%I:%M:%S %p";
	}

	namespace color
	{
		const u32 black = 0xff000000;
		const u32 blue = 0xff56BEFF;
		const u32 gray = 0xffbdc3c7;
		const u32 green = 0xff52c175;
		const u32 orange = 0xfffe8900;
		const u32 red = 0xffff1900;
		const u32 yellow = 0xfff1c40f;
		const u32 white = 0xffffffff;
	}

	namespace demonsaw
	{		
		const char* application = "Demonsaw";
		const char* author = "Eric J Anderson (Eijah)";		
		const char* copyright = "Copyright 2014-2016 Demonsaw, LLC All Rights Reserved";
		const char* date = "October 22th, 2016";
		const char* motto = "Believe in the Right to Share";
		const char* title = "Demonsaw 3.2.0 - Beta 3 (Malachite)";
		const char* titlebar = "Demonsaw 3.2.0 - Beta 3 (Malachite) [*]";
		const char* website = "https://www.demonsaw.com";
		const char* config = "demonsaw.toml";
		const char* font_awesome = "FontAwesome";
		const char* theme = "theme";
	}

	namespace group
	{
		const double percent = 100.0;
		const group_security_type type = group_security_type::none;

		const char* cipher = cipher_name::aes;
		const size_t key_size = static_cast<size_t>(cipher_bits::large);
		const char* hash = hash_name::sha256;
		const size_t iterations = pbkdf_sha256::get_min_iterations();
	}

	namespace io
	{
		const bool remove = true;
		const char* hash = hash_name::md5;
		const size_t stack_size = (64 << 10);
	}

	namespace milliseconds
	{
		const size_t socket = sec_to_ms(30);
		const size_t tunnel = min_to_ms(3);
		const size_t yield = 1;

		const size_t download = 250;
		const size_t upload = sec_to_ms(1);

		const size_t file = sec_to_ms(1);
		const size_t open = sec_to_ms(3);
		const size_t progress = sec_to_ms(1);
		const size_t queue = sec_to_ms(1);
		const size_t sync = sec_to_ms(10);
		const size_t timeout = min_to_ms(1);

		const size_t client = min_to_ms(5);
		const size_t server = min_to_ms(60);
		const size_t throttle = 10;
		const size_t transfer = min_to_ms(5);

		const size_t quit = sec_to_ms(1);
		const size_t restart = sec_to_ms(1); 
		const size_t retry = 500;
		const size_t status = 500;
	}

	namespace network
	{
		const size_t interval = sec_to_ms(1);
		const network_mode mode = network_mode::tunnel;
		const network_trust trust = network_trust::router;		

		const bool motd = true;
		const bool version = true;		

		const bool browse = true;
		const bool chat = true;
		const bool pm = true;
		const bool search = true;
		const bool upload = true;

		const size_t min_buffer_size = 1 << 20;
		const size_t max_buffer_size = 128 << 20;
		const size_t num_buffer_size = 16 << 20;

		const size_t min_chat = 1;
		const size_t max_chat = 4 << 10;
		const size_t num_chat = 4 << 10;

		const size_t min_chunk_size = 1 << 10;
		const size_t max_chunk_size = 4 << 20;
		const size_t num_chunk_size = 128 << 10;

		const size_t min_pm = 1;
		const size_t max_pm = 1 << 10;
		const size_t num_pm = 1 << 10;

		const size_t min_downloads = 1;
		const size_t max_downloads = 128;
		const size_t num_downloads = 16;

		const size_t min_retries = 0;
		const size_t max_retries = 32;
		const size_t num_retries = 16;

		const size_t min_routers = 1;
		const size_t max_routers = 8;
		const size_t num_routers = 4;

		const size_t min_sockets = 1;
		const size_t max_sockets = 4;
		const size_t num_sockets = 1;

		const size_t min_threads = 1;
		const size_t max_threads = std::max(static_cast<size_t>(1), static_cast<size_t>(std::thread::hardware_concurrency()));
		const size_t num_threads = std::max(min_threads, max_threads);

		const size_t min_timeout = min_to_ms(1);
		const size_t max_timeout = min_to_ms(3);
		const size_t num_timeout = min_to_ms(1);

		const size_t min_uploads = 1;
		const size_t max_uploads = 128;
		const size_t num_uploads = 16;

		const size_t min_window = 4;
		const size_t max_window = 128;
		const size_t num_window = 16;
	}

	namespace prime
	{
		const size_t size = 128;
		const size_t buffer = 0;
		const size_t reuse = 0;
		const size_t threshold = 0;
	}

	namespace router
	{
		const char* name = "[router]";
		const char* address = "0.0.0.0";
		const char* home = "router.demonsaw.com";
		const char* localhost = "127.0.0.1";
		const char* enigma = "enigma.eijah.com";

		const u16 port = 80;
		const u16 port_beta = 443;

		const bool message = true;
		const bool transfer = true;
		const bool proxy = true;
		const bool open = true;
	}

	namespace search
	{
		const char* delimiters = " ._-(){}[]'\"\t";
		const size_t min_text = 3;
		const size_t max_text = 128;
		const size_t max_history = (4 << 10);
	}

	namespace session
	{
		const char* algorithm = diffie_hellman::get_name();
		const size_t prime_size = diffie_hellman::get_size();

		const char* cipher = cipher_name::aes;
		const size_t key_size = static_cast<size_t>(cipher_bits::large);
		const char* hash = hash_name::sha256;
		const size_t iterations = pbkdf_sha256::get_min_iterations();
	}

	namespace ui
	{
		const size_t errors = 16 << 10;
		const size_t enter = sec_to_ms(2);
		const size_t refresh = sec_to_ms(1);

		const bool browse = true;
		const bool chat = true;
		const bool client = true;		
		const bool download = true;
		const bool error = true;
		const bool group = true;
		const bool help = true;
		const bool message = true;
		const bool option = true;
		const bool queue = true;
		const bool search = true;
		const bool session = true;
		const bool share = true;
		const bool status = true;
		const bool transfer = true;
		const bool upload = true;
	}

	namespace version
	{
		const size_t toml = 1;
		const size_t msgpack = 1;
		const version_component min(3, 0, 5);

		const char* app = "3.2.0";
		const size_t major = 3;
		const size_t minor = 2;
		const size_t patch = 0;
	}
}
