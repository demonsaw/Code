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

#include <sstream>

#include "http_code.h"

namespace eja
{
	std::ostream& operator<<(std::ostream& os, const http_code& code)
	{
		switch (code)
		{
			case http_code::ok:						{ return os << "OK"; }
			case http_code::moved:					{ return os << "Moved Permanently"; }
			case http_code::found:					{ return os << "Found"; }
			case http_code::see_other:				{ return os << "See Other"; }
			case http_code::bad_request:			{ return os << "Bad Request"; }
			case http_code::unauthorized:			{ return os << "Unauthorized"; }
			case http_code::forbidden:				{ return os << "Forbidden"; }
			case http_code::not_found:				{ return os << "Not Found"; }
			case http_code::method_not_allowed:		{ return os << "Method Not Allowed"; }
			case http_code::too_many_requests:		{ return os << "Too Many Requests"; }
			case http_code::login_timeout:			{ return os << "Login Timeout"; }
			case http_code::internal_server_error:	{ return os << "Internal Server Error"; }
			case http_code::not_implemented:		{ return os << "Not Implemented"; }
			case http_code::service_unavailable:	{ return os << "Service Unavailable"; }
			default:								{ return os << "Unknown Error"; }
		}
	}

	std::string str(const http_code& code)
	{
		std::ostringstream sout;
		sout << "HTTP " << static_cast<size_t>(code) << ": " << code;
		return sout.str();
	}
}
