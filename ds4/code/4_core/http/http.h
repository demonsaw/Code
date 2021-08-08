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

#ifndef _EJA_HTTP_H_
#define _EJA_HTTP_H_

#include <string>

#include "system/type.h"

namespace eja
{
	namespace http
	{
		static const char* eol = "\r\n";
		static const char* eom = "\r\n\r\n";
		static const char* space = " ";
		static const char* question = "?";
		
		static const char* get = "GET";
		static const char* post = "POST";
		static const std::string session_name = "id";
		static const char* param_name = "param";

		static const char* accept = "Accept";
		static const char* accept_encoding = "Accept-Encoding";
		static const char* connection = "Connection";
		static const char* content_length = "Content-Length";
		static const char* content_type = "Content-Type";
		static const char* host = "Host";
		static const char* location = "Location";

		static const char* all = "*/*";
		static const char* binary = "application/octet-stream";
		static const char* close = "close";
		static const char* identity = "identity;q=1.0, *;q=0";
		static const char* keep_alive = "keep-alive";
		static const char* resource = "/";
		static const char* version = "HTTP/1.1";		

		static const char* cookie = "Cookie: "; 
		static const char* set_cookie = "Set-Cookie: ";
	}
}

#endif
