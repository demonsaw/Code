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

#include "http_request.h"
#include "utility/boost.h"

namespace eja
{
	// Static
	const std::string http_request::s_cookie = "Cookie";

	// Constructor
	http_request::http_request()
	{ 
		set_length(0); 
		set_accept(http::accept_all);
	}
	
	http_request::http_request(const std::string& session) : http_message(session)
	{ 
		set_length(0);
		set_accept(http::accept_all);
	}
	
	http_request::http_request(const char* session) : http_message(session)
	{ 
		set_length(0);
		set_accept(http::accept_all);
	}

	// Interface
	void http_request::clear()
	{
		http_message::clear();

		m_method = http::post;
		m_resource = http::root;
		m_version = http::version;
		m_queries.clear();
	}

	void http_request::parse(const std::string& input)
	{
		clear();

		// Title
		std::string line;
		std::istringstream sstream(input);
		std::getline(sstream, line);
		boost::trim(line);

		if (line.empty())
			return;

		boost::string_tokenizer tokens(line, boost::char_separator<char>(" "));
		auto it = tokens.begin();

		// Bad Input
		if (it == tokens.end())
			return;

		// POST / HTTP/1.1
		std::string method = *it++;
		boost::trim(method);
		set_method(method);

		// Bad Input
		if (it == tokens.end())
			return;

		std::string resource = *it++;
		boost::trim(resource);
		set_resource(resource);

		// Bad Input
		if (it == tokens.end())
			return;

		std::string version = *it++;
		boost::trim(version);
		set_version(version);

		// Query
		size_t pos = resource.find_first_of('?');
		if (pos != std::string::npos)
		{
			std::string data = resource.substr(pos + 1);
			set_query(data);
		}

		// Header
		for (std::string line; std::getline(sstream, line); )
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

	std::string http_request::str() const
	{
		// POST /
		std::ostringstream sstream;
		sstream << m_method << ' ' << m_resource;

		// Query
		if (has_queries())
		{
			sstream << '?';

			size_t pos = m_queries.size();
			for (const auto& pair : m_queries)
			{
				sstream << pair.first << '=' << pair.second;
				if (--pos > 0)
					sstream << '&';
			}

			sstream << ' ';
		}

		// HTTP/1.1
		sstream << ' ' << m_version << http::eol;

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

	// Query
	void http_request::set_query(const std::string& data)
	{
		boost::char_separator<char> separator("&");
		boost::string_tokenizer tokens(data, separator);

		for (const std::string& token : tokens)
		{
			int pos = token.find_first_of('=');
			if (pos == std::string::npos)
				continue;

			key_type key = token.substr(0, pos);
			boost::trim(key);

			value_type value = token.substr(pos + 1);
			boost::trim(value);

			m_queries[key] = value;
		}
	}
}
