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

#ifndef _EJA_HTTP_REQUEST_H_
#define _EJA_HTTP_REQUEST_H_

#include <iostream>
#include <memory>
#include <string>

#include "http/http.h"
#include "http/http_message.h"

namespace eja
{
	class http_request final : public http_message
	{
		make_factory(http_request);

	private:
		std::string m_method = http::post;		
		std::string m_param;
		std::string m_param_name = http::param_name;
		std::string m_resource = http::resource;
		std::string m_session;
		std::string m_session_name;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<http_request>(); }

		// Is
		bool is_cookie(const char* psz) const { return boost::starts_with(psz, http::cookie); }
		bool is_header(const char* psz) const { return !is_cookie(psz); }

		// Set
		void set_session(const char* psz);

	public:
		http_request();
		explicit http_request(const std::string& method);

		// Operator
		friend std::ostream& operator<<(std::ostream& os, const http_request& obj) { return os << obj.str(); }

		// Interface
		virtual void clear() override;

		// Utility
		virtual std::string str() const override;

		// Has		
		//bool has_param() const { return !m_param.empty(); }
		bool has_session() const { return !m_session.empty(); }
		//bool has_session_name() const { return !m_session_name.empty(); }

		// Is
		bool is_get() const { return m_method == http::get; }
		bool is_post() const { return m_method == http::post; }

		// Set
		virtual void set(const char* input, const size_t input_size) override;

		void set_host(const std::string& host, const u16 port);
		void set_method(const std::string& method) { m_method = method; }
		//void set_param(const std::string& param) { m_param = param; }
		//void set_param_name(const std::string& param_name) { m_param_name = param_name; }
		//void set_resource(const std::string& resource) { m_resource = resource; }
		void set_session(const std::string& session) { m_session = session; }
		//void set_session_name(const std::string& name) { m_session_name = name; }

		// Get
		//const std::string& get_method() const { return m_method; }
		//const std::string& get_param() const { return m_param; }
		//const std::string& get_param_name() const { return m_param_name; }
		//const std::string& get_resource() const { return m_resource; }
		const std::string& get_session() const { return m_session; }
		//const std::string& get_session_name() const { return m_session_name; }

		// Static
		static ptr create(const std::string& method) { return std::make_shared<http_request>(method); }
	};
}

#endif
