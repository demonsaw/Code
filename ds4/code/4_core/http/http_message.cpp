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

#include <boost/algorithm/string.hpp>

#include "http/http_message.h"

namespace eja
{
	// Operator
	http_message& http_message::operator=(const http_message& obj)
	{
		if (this != &obj)
		{
			m_body = obj.m_body;
			m_headers = obj.m_headers;
		}

		return *this;
	}

	// Interface
	void http_message::clear()
	{
		m_body.clear();
		m_headers.clear();
	}

	// Set
	void http_message::set_body(std::string&& body)
	{
		m_body = std::move(body);

		set_length(m_body.size());
	}

	void http_message::set_body(const std::string& body)
	{
		m_body = body;

		set_length(m_body.size());
	}

	void http_message::set_body(const char* body, const size_t body_size)
	{
		m_body.assign(body, body_size);

		set_length(m_body.size());
	}

	void http_message::set_header(const char* psz)
	{
		auto pos = strchr(psz, ':');
		if (!pos)
			return;

		auto key = std::string(psz, pos - psz);
		boost::trim(key);
		if (key.empty())
			return;

		auto value = std::string(pos + sizeof(char));
		boost::trim(value);
		if (!value.empty())
			m_headers[key] = value;
	}

	void http_message::set_header(const map_type& header_map)
	{
		for (const auto& pair : header_map)
			set_header(pair.first, pair.second);
	}
}
