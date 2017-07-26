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

#include <cassert>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "http_response.h"
#include "system/type.h"
#include "utility/boost.h"

namespace eja
{
	// Static
	const std::string http_response::s_cookie = "Set-Cookie";

	// Interface
	void http_response::clear()
	{
		http_message::clear();

		m_status = http_code::ok;
		m_version = http::version;
	}

	void http_response::parse(const std::string& input)
	{
		clear();

		// Title
		std::string line;
		std::istringstream sstream(input);
		std::getline(sstream, line);
		boost::trim(line);

		if (line.empty())
			return;

		// Http/1.1 200 OK
		int offset = 0;
		int pos = line.find_first_of(' ', offset);
		if (pos == std::string::npos)
			return;

		std::string version = line.substr(offset, pos);
		boost::trim(version);

		offset = pos + 1;
		pos = line.find_first_of(' ', offset);
		if (pos == std::string::npos)
			return;

		std::string code = line.substr(offset, pos - offset);
		boost::trim(code);
		set_status(static_cast<http_code>(boost::lexical_cast<int>(code)));

		offset = pos + 1;
		std::string status = line.substr(offset);
		boost::trim(status);

		// Header
		for (std::string line; std::getline(sstream, line);)
		{
			boost::trim(line);
			if (line.empty())
				break;

			if (is_header(line))
				set_header(line);
			else
				set_cookie(line);
		}
	}

	std::string http_response::str() const
	{		
		// Http/1.1 200 OK
		std::ostringstream sstream;
		sstream << m_version << ' ' << static_cast<u32>(m_status.get_code()) << ' ' << m_status << http::eol;

		// Header
		for (const auto& pair : m_headers)
			sstream << pair.first << ": " << pair.second << http::eol;

		// Session
		if (has_session())
			sstream << s_cookie << ": " << http::session << '=' << m_session << http::eol;

		// Cookies
		for (const auto& pair : m_cookies)
			sstream << s_cookie << ": " << pair.first << '=' << pair.second << http::eol;

		sstream << http::eol << m_body;

		return sstream.str();
	}
}
