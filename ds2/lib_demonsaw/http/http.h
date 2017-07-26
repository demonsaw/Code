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

#ifndef _EJA_HTTP_
#define _EJA_HTTP_

#include <vector>

#include "system/type.h"

namespace eja
{
	namespace http
	{
		// General
		static const char* eol = "\r\n";
		static const char* eom = "\r\n\r\n";
		static const size_t buffer_size = (4 << 10);		

		// Data		
		static const char* get = "GET"; 
		static const char* post = "POST";		
		static const char* delimiter = "/";
		static const char* root = delimiter;		
		static const char* version_1_0 = "HTTP/1.0";
		static const char* version_1_1 = "HTTP/1.1"; 
		static const char* version = version_1_1;
		static const char* keep_alive = "keep-alive";
		static const char* application = "application/x-www-form-urlencoded";
		static const char* text = "text/plain";

		static const char* scheme = "http";
		static const char* scheme_secure = "https";
		static const char* scheme_delimiter = "://";
		static const u16 port = 80;

		// Cookies
		static const char* session = "session";
		static const std::vector<const char*> cookies = { "Cookie", "Set-Cookie" };

		// Headers
		static const char* host = "Host";
		static const char* accept = "Accept";
		static const char* length = "Content-Length";
		static const char* transfer_encoding = "Transfer-Encoding";		
		static const char* type = "Content-Type";
		static const char* connection = "Connection";
		static const char* location = "Location";

		// Data
		static const char* accept_all = "*/*";
		static const char* transfer_encoding_chunked = "chunked";
	};
}

#endif
