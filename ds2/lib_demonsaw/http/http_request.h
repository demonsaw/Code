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

#ifndef _EJA_HTTP_REQUEST_
#define _EJA_HTTP_REQUEST_

#include <memory>
#include <string>

#include "http_message.h"

namespace eja
{
	class http_request final : public http_message
	{
	private:
		std::string m_method = http::post;
		std::string m_resource = http::root;
		std::string m_version = http::version;
		map_type m_queries;

		static const std::string s_cookie;

	public:
		using ptr = std::shared_ptr<http_request>;

	protected:
		// Query
		void set_query(const std::string& data);

	public:
		http_request();
		http_request(const std::string& session);
		http_request(const char* session);
		virtual ~http_request() override { }

		// Interface
		virtual void clear() override;
		virtual std::string str() const override; 
		virtual void parse(const std::string& input) override;
		virtual void parse(const char* input) override  { parse(std::string(input)); }

		// Utility
		virtual bool valid() const override { return http_message::valid() && has_headers() && has_body(); }

		bool has_host() const { return has_header(http::host); }
		bool has_accept() const { return has_header(http::accept); }

		bool is_get() const { return http::get == get_method(); }
		bool is_post() const { return http::post == get_method(); }
		bool is_root() const { return http::root == get_resource(); }

		// Query
		bool has_queries() const { return !m_queries.empty(); }
		const map_type& get_queries() const { return m_queries; }

		bool has_query(const key_type& key) const { return m_queries.find(key) != m_queries.end(); }
		template <typename T> void set_query(const key_type& key, const T& value);
		template <typename T> T get_query(const key_type& key) const;
		template <typename T> T get_query(const key_type& key, const T& value) const;
		value_type get_query(const key_type& key) const { return get_query<value_type>(key, empty_string()); }

		// Mutator
		void set_method(const std::string& method) { m_method = method; }
		void set_resource(const std::string& resource) { m_resource = resource; }
		void set_version(const std::string& version) { m_version = version; }
		void set_host(const std::string& host) { set_header(http::host, host); }
		void set_accept(const std::string& accept) { set_header(http::accept, accept); }

		// Accessor
		const std::string& get_method() const { return m_method; }
		const std::string& get_resource() const { return m_resource; }
		const std::string& get_version() const { return m_version; }
		std::string get_host() const { return get_header(http::host); }
		std::string get_accept() const { return get_header(http::accept); }

		// Static
		static ptr create() { return std::make_shared<http_request>(); }
		static ptr create(const std::string& session) { return std::make_shared<http_request>(session); }
		static ptr create(const char* session) { return std::make_shared<http_request>(session); }
	};

	// Query
	template <typename T>
	void http_request::set_query(const key_type& key, const T& value)
	{
		m_queries[key] = boost::lexical_cast<value_type>(value);
	}

	template <typename T>
	T http_request::get_query(const key_type& key) const
	{
		auto it = m_queries.find(key);
		return boost::lexical_cast<T>(it->second);
	}

	template <typename T>
	T http_request::get_query(const key_type& key, const T& value) const
	{
		auto it = m_queries.find(key);
		return (it != m_queries.end()) ? it->second : value;
	}
}

#endif
