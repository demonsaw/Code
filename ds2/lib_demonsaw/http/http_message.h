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

#ifndef _EJA_HTTP_MESSAGE_
#define _EJA_HTTP_MESSAGE_

#include <map>
#include <memory>
#include <string>
#include <boost/lexical_cast.hpp>

#include "http.h"
#include "security/base.h"
#include "system/state.h"
#include "system/type.h"

namespace eja
{
	class http_message : public type
	{
	protected:
		using key_type = std::string;
		using value_type = std::string;
		using map_type = std::map<key_type, value_type>;

	protected:		
		std::string m_body;
		std::string m_session;
		map_type m_headers;
		map_type m_cookies;

	public:
		using ptr = std::shared_ptr<http_message>;

	protected:
		http_message();
		http_message(const std::string& session) : http_message() { set_session(session); }
		http_message(const char* session) : http_message() { set_session(session); }
		virtual ~http_message() override { }

	protected:
		// Cookie
		bool is_cookie(const std::string& key) const;
		void set_cookie(const std::string& data);		

		// Header
		bool is_header(const std::string& key) const { return !is_cookie(key); }
		void set_header(const std::string& data);

	public:
		// Operator
		friend std::ostream& operator<<(std::ostream& os, const http_message& http) { return os << http.str(); }
		operator std::string() { return str(); }

		// Interface
		virtual void clear();
		virtual std::string str() const override;
		virtual void parse(const std::string& input) = 0;
		virtual void parse(const char* input) = 0;

		// Utility
		bool has_body() const { return !m_body.empty(); }
		bool has_session() const { return !m_session.empty(); }
		bool has_length() const { return get_length() > 0; }		
		bool has_type() const { return has_header(http::type); }
		bool has_connection() const { return has_header(http::connection); }
		bool has_location() const { return has_header(http::location); }
		bool has_keep_alive() const { return get_connection() == http::keep_alive; }
		bool has_transfer_encoding() const { return has_header(http::transfer_encoding); }		

		// Cookie
		bool has_cookies() const { return !m_cookies.empty(); }
		const map_type& get_cookies() const { return m_cookies; }

		bool has_cookie(const key_type& key) const { return m_cookies.find(key) != m_cookies.end(); }
		template <typename T> void set_cookie(const key_type& key, const T& value);
		template <typename T> T get_cookie(const key_type& key) const;
		template <typename T> T get_cookie(const key_type& key, const T& value) const;
		value_type get_cookie(const key_type& key) const { return get_cookie<value_type>(key, empty_string()); }

		// Header
		bool has_headers() const { return !m_headers.empty(); }
		const map_type& get_headers() const { return m_headers; }

		bool has_header(const key_type& key) const { return m_headers.find(key) != m_headers.end(); }
		template <typename T> void set_header(const key_type& key, const T& value);
		template <typename T> T get_header(const key_type& key) const;
		template <typename T> T get_header(const key_type& key, const T& value) const;
		value_type get_header(const key_type& key) const { return get_header<value_type>(key, empty_string()); }

		// Mutators		
		void set_body(const char* body, const size_t body_size);		
		void set_body(const char* body) { set_body(body, strlen(body)); }
		void set_body(const std::string& body) { set_body(body.c_str(), body.size()); }

		void set_session(const std::string& session) { m_session = session; }
		void set_session(const char* session) { m_session = session; }

		void set_connection(const std::string& socket) { set_header(http::connection, socket); }
		void set_location(const std::string& location) { set_header(http::location, location); }
		void set_length(const size_t length) { set_header(http::length, length); }
		void set_type(const std::string& type) { set_header(http::type, type); }

		// Accessors
		const std::string& get_body() const { return m_body; }		
		const std::string& get_session() const { return m_session; }
		std::string get_connection() const { return get_header(http::connection); }
		std::string get_location() const { return get_header(http::location); }
		size_t get_length() const { return get_header<size_t>(http::length, 0); }
		std::string get_type() const { return get_header(http::type); }
		std::string get_transfer_encoding() const { return get_header(http::transfer_encoding); }
	};

	// Cookie
	template <typename T>
	void http_message::set_cookie(const key_type& key, const T& value)
	{
		m_cookies[key] = boost::lexical_cast<value_type>(value);
	}
	
	template <typename T>
	T http_message::get_cookie(const key_type& key) const
	{
		auto it = m_headers.find(key);
		return boost::lexical_cast<T>(it->second);
	}

	template <typename T>
	T http_message::get_cookie(const key_type& key, const T& value) const
	{
		auto it = m_headers.find(key);
		return (it != m_headers.end()) ? boost::lexical_cast<T>(it->second) : value;
	}

	// Header
	template <typename T>
	void http_message::set_header(const key_type& key, const T& value)
	{
		m_headers[key] = boost::lexical_cast<value_type>(value);
	}
	
	template <typename T>
	T http_message::get_header(const key_type& key) const
	{
		auto it = m_headers.find(key);
		return boost::lexical_cast<T>(it->second);
	}

	template <typename T>
	T http_message::get_header(const key_type& key, const T& value) const
	{
		auto it = m_headers.find(key);
		return (it != m_headers.end()) ? boost::lexical_cast<T>(it->second) : value;
	}
}

#endif
