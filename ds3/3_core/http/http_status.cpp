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

#include "http_status.h"

namespace eja
{
	// Operator
	http_status& http_status::operator=(const http_status& status)
	{
		if (this != &status)
			m_code = status.m_code;

		return *this;
	}

	http_status& http_status::operator=(const http_code& code)
	{
		m_code = code;

		return *this;
	}

	// Set
	void http_status::set_code(const char* psz)
	{
		if (!strcmp(psz, "OK"))
			m_code = http_code::ok;
		else if (!strcmp(psz, "Not Found"))
			m_code = http_code::not_found;
		else if (!strcmp(psz, "See Other"))
			m_code = http_code::see_other;
		else if (!strcmp(psz, "Unauthorized"))
			m_code = http_code::unauthorized;
		else if (!strcmp(psz, "Not Implemented"))
			m_code = http_code::not_implemented;
		else if (!strcmp(psz, "Internal Server Error"))
			m_code = http_code::internal_service_error;
		else
			m_code = http_code::none;
	}

	// Get
	const char* http_status::get_description() const
	{
		switch (m_code)
		{
			case http_code::ok:							return "OK";
			case http_code::not_found:					return "Not Found";
			case http_code::see_other:					return "See Other";
			case http_code::unauthorized:				return "Unauthorized";
			case http_code::not_implemented:			return "Not Implemented";
			case http_code::internal_service_error:		return "Internal Server Error";
			default:									return "None";
		}
	}
}
