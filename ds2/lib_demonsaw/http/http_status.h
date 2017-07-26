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

#ifndef _EJA_HTTP_STATUS_
#define _EJA_HTTP_STATUS_

#include <iostream>

#include "http_code.h"

namespace eja
{
	class http_status final
	{
	private:
		http_code m_code;

	public:
		http_status() : m_code(http_code::none) { }
		http_status(const http_code& code) : m_code(code) { }
		~http_status() { }

		// Operator
		friend std::ostream& operator<<(std::ostream& os, const http_status& status) { return os << status.get_code(); }
		http_status& operator=(const http_code& code);

		// Utility
		std::string str() const { return eja::str(m_code); }

		// Status
		bool is_none() const { return m_code == http_code::none; }
		bool is_ok() const { return m_code == http_code::ok; }
		bool is_error() const { return !is_ok(); }

		bool is_moved() const { return m_code == http_code::moved; }
		bool is_found() const { return m_code == http_code::found; }
		bool is_see_other() const { return m_code == http_code::see_other; }
		bool is_redirect() const { return is_moved() || is_found() || is_see_other(); }

		bool is_bad_request() const { return m_code == http_code::bad_request; }
		bool is_unauthorized() const { return m_code == http_code::unauthorized; }
		bool is_forbidden() const { return m_code == http_code::forbidden; }
		bool is_not_found() const { return m_code == http_code::not_found; }
		bool is_too_many_requests() const { return m_code == http_code::too_many_requests; }
		bool is_login_timeout() const { return m_code == http_code::login_timeout; }

		bool is_internal_server_error() const { return m_code == http_code::internal_server_error; }
		bool is_not_implemented() const { return m_code == http_code::not_implemented; }
		bool is_service_unavailable() const { return m_code == http_code::service_unavailable; }

		// Mutator
		void set_code(const http_code& code) { m_code = code; }

		// Accessor
		const http_code& get_code() const { return m_code; }
		http_code& get_code() { return m_code; }
	};
}

#endif
