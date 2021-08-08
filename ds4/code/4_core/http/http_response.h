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

#ifndef _EJA_HTTP_RESPONSE_H_
#define _EJA_HTTP_RESPONSE_H_

#include <iostream>
#include <string>

#include "http/http_code.h"
#include "http/http_message.h"
#include "http/http_status.h"

namespace eja
{
	class http_response final : public http_message
	{
		make_factory(http_response);

	private:
		http_status m_status;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<http_response>(); }

	public:
		http_response() : http_response(http_code::none) { }
		explicit http_response(const http_status& status) : m_status(status) { }
		explicit http_response(const http_code code) : m_status(code) {  }

		// Operator
		friend std::ostream& operator<<(std::ostream& os, const http_response& obj) { return os << obj.str(); }

		// Interface
		virtual void clear() override;

		// Utility
		virtual std::string str() const override;

		// Is		
		bool is_ok() const { return m_status.is_ok(); }
		bool is_error() const { return m_status.is_error(); }

		bool is_redirect() const { return m_status.is_redirect(); }
		bool is_unauthorized() const { return m_status.is_unauthorized(); }
		bool is_not_found() const { return m_status.is_not_found(); }
		bool is_not_implemented() const { return m_status.is_not_implemented(); }

		// Set
		virtual void set(const char* input, const size_t input_size) override;

		void set_location(std::string&& location) { set_header(http::location, std::move(location)); }
		void set_location(const std::string& location) { set_header(http::location, location); }

		void set_status(const http_status& status) { m_status = status; }
		void set_code(const http_code code) { m_status = code; }

		// Get
		const char* get_description() const { return m_status.get_description(); }
		const http_status& get_status() const { return m_status; }
		http_code get_code() const { return m_status.get_code(); }		

		// Static
		static ptr create(const http_status& status) { return std::make_shared<http_response>(status); }
		static ptr create(const http_code code) { return std::make_shared<http_response>(code); }
	};
}

#endif
