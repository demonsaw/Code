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

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include <boost/algorithm/string.hpp>

#include "http/http.h"
#include "http/http_response.h"
#include "security/filter/base.h"

namespace eja
{
	// Operator
	http_response& http_response::operator=(const http_response& response)
	{
		if (this != &response)
		{
			http_message::operator=(response);

			m_status = response.m_status;
		}

		return *this;
	}

	// Interface
	void http_response::clear()
	{
		http_message::clear();

		m_status = http_code::none;		
	}

	void http_response::set(const char* input, const size_t input_size)
	{
		clear();

		// Http/1.1 200 OK
		auto psz = strtok(const_cast<char*>(input) , http::space);
		if (!psz)
			return;

		size_t size = strlen(psz) + strlen(http::space);
		psz = strtok(const_cast<char*>(input) + size, http::space);
		assert((psz - input) == size);
		if (!psz)
			return;

		const auto code = static_cast<http_code>(boost::lexical_cast<size_t>(psz));
		set_status(code);

		size += strlen(psz) + strlen(http::space);
		psz = strtok(const_cast<char*>(input) + size, http::eol);
		assert((psz - input) == size);
		if (!psz)
			return;

		size += strlen(psz) + strlen(http::eol);

		while ((size + strlen(http::eom)) < input_size)
		{
			psz = strtok(const_cast<char*>(input) + size, http::eol);
			assert((psz - input) == size);
			
			if (is_header(psz))
				set_header(psz);
			else if (!has_session())
				set_cookie(psz);

			size += strlen(psz) + strlen(http::eol);
		}
	}

	std::string http_response::str() const
	{		
		// Http/1.1 200 OK
		std::ostringstream sstream;

		// Version
		sstream << http::version << ' ';
		
		// Status
		sstream << m_status.get_code() << ' ' << m_status.get_description() << http::eol;

		// Header
		for (const auto& pair : m_headers)
			sstream << pair.first << ": " << pair.second << http::eol;

		// Session
		if (has_session())
			sstream << http::set_cookie << m_session_name << '=' << base64::encode(m_session) << http::eol;

		// Empty line
		sstream << http::eol;

		// Body
		if (has_body())
			sstream << m_body;

		return sstream.str();
	}
}
