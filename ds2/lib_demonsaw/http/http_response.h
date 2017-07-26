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

#ifndef _EJA_HTTP_RESPONSE_
#define _EJA_HTTP_RESPONSE_

#include <memory>
#include <string>

#include "http_message.h"
#include "http_status.h"

namespace eja
{
	class http_response : public http_message
	{
	private:		
		http_status m_status = http_code::none;
		std::string m_version = http::version;

		// Static
		static const std::string s_cookie;

	public:
		using ptr = std::shared_ptr<http_response>;

	public:
		http_response() { }
		http_response(const http_code& code) : m_status(code) { }
		http_response(const http_status& status) : m_status(status) { }
		http_response(const std::string& session) : http_message(session) { }
		http_response(const char* session) : http_message(session) { }
		http_response(const std::string& session, const http_status& status) : http_message(session), m_status(status) { }
		http_response(const char* session, const http_status& status) : http_message(session), m_status(status) { }
		virtual ~http_response() override { }

		// Interface
		virtual void clear() override;
		virtual std::string str() const override;
		virtual void parse(const std::string& input) override;
		virtual void parse(const char* input) override  { parse(std::string(input)); }

		// Utility
		virtual bool valid() const override { return http_message::valid() && m_status.is_ok(); }

		// Mutator
		void set_status(const http_status& status) { m_status = status; }
		void set_version(const std::string& version) { m_version = version; }

		// Accessor
		const http_status& get_status() const { return m_status; }
		const std::string& get_version() const { return m_version; }

		// Static
		static ptr create() { return std::make_shared<http_response>(); }
		static ptr create(const http_status& status) { return std::make_shared<http_response>(status); }
		static ptr create(const std::string& session) { return std::make_shared<http_response>(session); }		
		static ptr create(const std::string& session, const http_status& status) { return std::make_shared<http_response>(session, status); }
		static ptr create(const char* session) { return std::make_shared<http_response>(session); }
		static ptr create(const char* session, const http_status& status) { return std::make_shared<http_response>(session, status); }
	};

	class http_ok_response final : public http_response
	{
	public:
		http_ok_response() : http_response(http_code::ok) { }
		http_ok_response(const std::string& session) : http_response(session, http_code::ok) { }
		http_ok_response(const char* session) : http_response(session, http_code::ok) { }
		virtual ~http_ok_response() override { }
	};
}

#endif
