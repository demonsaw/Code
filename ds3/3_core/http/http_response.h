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

		// Utility
		virtual bool is_header(const char* psz) const override { return !boost::starts_with(psz, http::set_cookie); }

	public:
		http_response() : http_response(http_code::none) { }
		http_response(const http_response& response) : http_message(response), m_status(response.m_status) { }
		explicit http_response(const http_status& status) : m_status(status) { }
		explicit http_response(const http_code code) : m_status(code) {  }

		// Operator
		friend std::ostream& operator<<(std::ostream& os, const http_response& response) { return os << response.str(); }
		http_response& operator=(const http_response& response);

		// Interface
		virtual void clear() override;

		// Utility
		virtual std::string str() const override;

		// Set
		virtual void set(const char* input, const size_t input_size) override;
		void set_status(const http_status& status) { m_status = status; }
		void set_status(const http_code code) { m_status = code; }

		// Get
		const http_status& get_status() const { return m_status; }	

		// Static
		static ptr create(const http_status& status) { return ptr(new http_response(status)); }
		static ptr create(const http_code code) { return ptr(new http_response(code)); }
	};
}

#endif
