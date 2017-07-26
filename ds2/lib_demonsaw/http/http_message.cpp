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

#include <algorithm>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "http_message.h"
#include "system/type.h"
#include "utility/boost.h"

namespace eja
{
	// Constructor
	http_message::http_message()
	{
		set_connection(http::keep_alive);		
		set_type(http::text);
	}

	// Interface
	void http_message::clear()
	{
		m_body.clear();
		m_headers.clear();
		m_cookies.clear();

		set_connection(http::keep_alive);
		set_type(http::text);
	}

	// Utility
	std::string http_message::str() const
	{
		std::ostringstream sstream;

		for (const auto& pair : m_headers)
			sstream << pair.first << ": " << pair.second << http::eol;

		return sstream.str();
	}

	// Cookie
	bool http_message::is_cookie(const std::string& key) const
	{
		for (const auto& cookie : http::cookies)
		{
			if (boost::starts_with(key, cookie))
				return true;
		}

		return false;
	}

	void http_message::set_cookie(const std::string& data)
	{
		size_t pos = data.find_first_of(':');
		const std::string suffix = (pos != std::string::npos) ? data.substr(pos + 1) : data;

		boost::char_separator<char> separator(";");
		boost::string_tokenizer tokens(suffix, separator);

		for (const auto& line : tokens)
		{
			int pos = line.find_first_of('=');
			if (pos == std::string::npos)
				continue;

			key_type key = line.substr(0, pos);
			boost::trim(key);

			value_type value = line.substr(pos + 1);
			boost::trim(value);

			// TODO: Do we need to decode this?
			if (key == http::session)
				set_session(value);
			else			
				m_cookies[key] = value;
		}
	}
	
	// Header
	void http_message::set_header(const std::string& data)
	{
		int pos = data.find_first_of(':');
		if (pos == std::string::npos)
			return;

		key_type key = data.substr(0, pos);
		boost::trim(key);

		value_type value = data.substr(pos + 1);
		boost::trim(value);

		m_headers[key] = value;
	}

	// Mutators
	void http_message::set_body(const char* body, const size_t body_size)
	{
		m_body.assign(body, body_size);;

		set_length(m_body.size());
	}
}
