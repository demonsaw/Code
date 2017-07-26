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
#include "http/http_request.h"
#include "security/filter/base.h"
#include "security/filter/url.h"

namespace eja
{
	// Operator
	http_request& http_request::operator=(const http_request& request)
	{
		if (this != &request)
		{
			http_message::operator=(request);

			m_method = request.m_method;			
			m_param = request.m_param;
			m_param_name = request.m_param_name;
			m_resource = request.m_resource;
		}			

		return *this;
	}

	// Interface
	void http_request::clear()
	{
		http_message::clear();

		m_method = http::post;
		m_param.clear();
		m_param_name = http::param_name;
		m_resource = http::resource;		
	}

	// Utility
	std::string http_request::str() const
	{
		// POST / HTTP/1.1
		std::ostringstream sstream;

		// Method
		sstream << m_method << ' ';

		// Resource
		sstream << m_resource;

		// Param
		if (has_param())
		{
			const auto base64_encoded = base64::encode(m_param);
			const auto url_encoded = url::encode(base64_encoded);
			sstream << '?' << m_param_name << '=' << url_encoded;
		}

		// Version
		sstream << ' ' << http::version << http::eol;

		// Header
		for (const auto& pair : m_headers)
			sstream << pair.first << ": " << pair.second << http::eol;

		// Session
		if (has_session())
			sstream << http::cookie << m_session_name << '=' << base64::encode(m_session) << http::eol;

		// Empty line
		sstream << http::eol;

		// Body
		if (has_body())
			sstream << m_body;

		return sstream.str();
	}

	// Set
	void http_request::set(const char* input, const size_t input_size)
	{
		clear();

		// POST / HTTP/1.1
		auto psz = strtok(const_cast<char*>(input), http::space);
		if (!psz)
			return;

		// Method
		m_method = psz;
		boost::trim(m_method);
		size_t size = strlen(psz) + strlen(http::space);

		psz = strtok(const_cast<char*>(input) + size, http::space);
		assert((psz - input) == size); 
		if (!psz)
			return;
		
		const auto key = strchr(psz, '?');

		if (!key)
		{
			// Resource
			m_resource = psz;
			boost::trim(m_resource);
			size += strlen(psz) + strlen(http::question);
		}
		else
		{
			// Query String
			m_resource.assign(psz, key - psz);
			boost::trim(m_resource);
			size += strlen(psz) + strlen(http::question);

			const auto value = strchr(key + strlen(http::question), '=');
			if (!value)
				return;

			// Key
			m_param_name.assign(key + 1, value - (key + 1));

			// Value
			const auto url_encoded = std::string(value + 1, strlen(psz) - (m_param_name.size() + strlen(http::question)));
			const auto base64_encoded = url::decode(url_encoded);
			m_param = base64::decode(base64_encoded);
		}

		psz = strtok(const_cast<char*>(input) + size, http::eol);
		assert((psz - input) == size);
		if (!psz)
			return;

		// Version
		size += strlen(psz) + strlen(http::eol);

		// Headers
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
}
