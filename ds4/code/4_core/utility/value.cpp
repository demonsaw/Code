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
#include "component/group/group_option_component.h"

#include "security/security.h"
#include "security/algorithm/diffie_hellman.h"
#include "security/cipher/aes.h"
#include "security/hash/sha.h"
#include "utility/value.h"

namespace eja
{
	namespace browse
	{
		const size_t min_rows = 64;
		const size_t max_rows = 4096;
	}

	namespace chat
	{
		const size_t max_rows = (16 << 10);
		const size_t max_text = (16 << 10);
		const size_t min_text = 1;
	}

	namespace client
	{
		const char* name = "[client]";
		const char* timestamp = "%I:%M:%S %p";

		const size_t min_ring = 0;
		const size_t max_ring = 1 << 10;
		const size_t num_ring = 128;
	}

	namespace color
	{
		const u32 black = 0xff000000;
		const u32 blue = 0xff0c9bdc;
		const u32 gray = 0xffbdc3c7;
		const u32 green = 0xff52c175;
		const u32 orange = 0xffdc4d0c;
		const u32 red = 0xffcb1511;
		const u32 yellow = 0xfff1c40f;
		const u32 white = 0xffffffff;
	}

	namespace download
	{
		const size_t pending = sec_to_ms(3);
		const size_t timeout = sec_to_ms(30);

		const size_t min_throttle_size = 0;
		const size_t max_throttle_size = 1 << 10;
		const size_t num_throttle_size = 0;

		const size_t max_retry = 6;
	}

	namespace group
	{
		const double percent = 100.0;
		const entropy_type type = entropy_type::none;

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

		const size_t max_id_size = security::get_max_size();
		const size_t max_name_size = 64;
		const size_t max_color_size = 8;

		const size_t max_path_size = 255 WIN32_ONLY(+ 5);
		const size_t max_address_size = 64;
		const size_t max_port_size = 5;

		const size_t max_cipher_size = 8;
		const size_t max_hash_size = 8;
		const size_t max_key_size = (4 << 10);
		const size_t max_passphrase_size = 256;
		const size_t max_salt_size = 256;
	}

	namespace milliseconds
	{
		const size_t error = sec_to_ms(1);
		const size_t socket = sec_to_ms(15);
		const size_t timeout = min_to_ms(5);
		const size_t tunnel = min_to_ms(3);
		const size_t monitor = min_to_ms(1);
		const size_t yield = 0;

		const size_t ping = min_to_ms(1);
		const size_t file = sec_to_ms(1);
		const size_t open = sec_to_ms(3);
		const size_t progress = sec_to_ms(1);
		const size_t sync = sec_to_ms(10);

		const size_t quit = sec_to_ms(1);
		const size_t restart = sec_to_ms(1);
		const size_t retry = 500;
		const size_t statusbar = 200;
	}

	namespace network
	{
		const size_t min_buffer_size = 4;
		const size_t max_buffer_size = 128;
		const size_t num_buffer_size = 16;

		const size_t min_chunk_size = 4 << 10;		//   4 KB
		const size_t max_chunk_size = 4 << 20;		//	 4 MB
		const size_t num_chunk_size = 768 << 10;	// 768 KB
	}

	namespace pm
	{
		const size_t max_text = (16 << 10);
		const size_t min_text = 1;
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
		const char* name = "Router";
		const char* address = "router.demonsaw.com";
		const char* password = "";
		const u16 port = 80;

		const size_t min_ring = 0;
		const size_t max_ring = 1 << 10;
		const size_t num_ring = 128;

		const bool message = true;
		const bool transfer = true;
		const bool proxy = true;
		const bool open = true;
	}

	namespace search
	{
		const char* delimiters = " ._(){}[]\"\t";

		const size_t min_rows = 64;
		const size_t max_rows = 4096;

		const size_t min_text = 3;
		const size_t max_text = 128;
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

	namespace share
	{
		const size_t min_text = 3;
	}

	namespace software
	{
		const char* name = "Demonsaw";
		const char* author = "An Eijah Production";
		const char* motto = "Believe in the Right to Share";

		const char* version = "4.20";
		const char* build = "Version 4.20";
		const char* date = "April 20th, 2020";
		const char* title = "Demonsaw 4.20";
		const char* titlebar = "Demonsaw 4.20 [*]";

		const char* website = "https://www.demonsaw.com";
		const char* help = "https://www.titan.wiki";
		const char* config = "demonsaw.toml";
		const char* font_awesome = "FontAwesome";
		const char* theme = "theme";

		const u32 min_version = 1;
		const u32 max_version = u32_max;
		const u32 num_version = 1;
	}

	namespace suffix
	{
		const char* troll = " [TROLL]";
	}

	namespace transfer
	{
		const size_t queue = sec_to_ms(1);
		const size_t wait = 100;
	}

	namespace ui
	{
		const size_t errors = 4 << 10;
		const size_t enter = sec_to_ms(1);

		const size_t min_font_size = 10;
		const size_t max_font_size = 36;
		const size_t num_font_size = 0;

		const size_t min_tabs = 1;
		const size_t max_tabs = 16;
		const size_t num_tabs = 8;

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

	namespace upload
	{
		const size_t pending = sec_to_ms(3);
		const size_t timeout = sec_to_ms(30);

		const size_t min_throttle_size = 0;
		const size_t max_throttle_size = 1 << 10;
		const size_t num_throttle_size = 0;
	}
}
