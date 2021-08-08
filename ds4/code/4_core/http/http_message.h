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

#ifndef _EJA_HTTP_MESSAGE_H_
#define _EJA_HTTP_MESSAGE_H_

#include <map>
#include <memory>
#include <string>
#include <boost/lexical_cast.hpp>

#include "http/http.h"
#include "object/object.h"
#include "system/type.h"

namespace eja
{
	class http_message : public object, public std::enable_shared_from_this<http_message>
	{
	protected:
		using key_type = std::string;
		using value_type = std::string;
		using map_type = std::map<key_type, value_type>;

	protected:
		std::string m_body;		
		map_type m_headers;

	protected:
		http_message() { }
		http_message(const http_message& obj) : m_body(obj.m_body), m_headers(obj.m_headers) { }

		// Operator
		http_message& operator=(const http_message& obj);

		// Interface
		template <typename T>
		std::shared_ptr<T> shared_from_base() { return std::static_pointer_cast<T>(shared_from_this()); }

		// Has
		bool has_header(const key_type& key) const { return m_headers.find(key) != m_headers.end(); }

		// Set
		void set_header(const char* psz);
		void set_length(const size_t length) { set_header(http::content_length, length); }

		// Get
		template <typename T> T get_header(const key_type& key, const T& value) const;
		template <typename T> T get_header(const key_type& key) const { return get_header(key, 0); }
		value_type get_header(const key_type& key) const { return get_header<value_type>(key, empty_string()); }

	public:		
		// Interface
		virtual void clear();

		// Utility
		virtual std::string str() const = 0;
		virtual string_ptr str_ptr() const { return string_ptr(new std::string(std::move(str()))); }

		// Has
		bool has_body() const { return !m_body.empty(); }
		bool has_length() const { return has_header(http::content_length); }

		// Set
		virtual void set(const char* input, const size_t input_size) = 0;

		void set_body(std::string&& body);
		void set_body(const std::string& body);
		void set_body(const char* body, const size_t body_size);
		void set_body(const char* body) { set_body(body, strlen(body)); }

		template <typename T> void set_header(const key_type& key, T&& value) { m_headers[key] = boost::lexical_cast<value_type>(std::move(value)); }
		template <typename T> void set_header(const key_type& key, const T& value) { m_headers[key] = boost::lexical_cast<value_type>(value); }
		void set_header(const map_type& header_map);

		// Get
		const std::string& get_body() const { return m_body; }
		size_t get_length() const { return get_header<size_t>(http::content_length, 0); }
	};

	// Get
	template <typename T>
	T http_message::get_header(const key_type& key, const T& value) const
	{
		auto it = m_headers.find(key);
		return (it != m_headers.end()) ? boost::lexical_cast<T>(it->second) : value;
	}
}

#endif
