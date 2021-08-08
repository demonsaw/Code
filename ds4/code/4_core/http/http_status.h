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

#ifndef _EJA_HTTP_STATUS_H_
#define _EJA_HTTP_STATUS_H_

#include <string>

#include "http/http_code.h"
#include "system/type.h"

namespace eja
{	
	class http_status final
	{
		make_factory(http_status);

	private:
		http_code m_code = http_code::none;

	public:
		http_status() { }
		http_status(const http_status& status) : m_code(status.m_code) { }
		explicit http_status(const http_code& code) : m_code(code) { }
		explicit http_status(const std::string& str) { set_code(str); }
		explicit http_status(const char* psz) { set_code(psz); }

		// Operator
		http_status& operator=(const http_status& status);
		http_status& operator=(const http_code& code);

		// Is		
		bool is_ok() const { return m_code == http_code::ok; }
		bool is_error() const { return !is_ok(); }
		
		bool is_redirect() const { return m_code == http_code::redirect; }
		bool is_unauthorized() const { return m_code == http_code::unauthorized; }
		bool is_not_found() const { return m_code == http_code::not_found; }		
		bool is_not_implemented() const { return m_code == http_code::not_implemented; }

		// Has
		bool has_code() const { return m_code != http_code::none; }

		// Set
		void set_code(const http_code& code) { m_code = code; }
		void set_code(const std::string& code) { set_code(code.c_str()); }
		void set_code(const char* code);

		// Get
		const http_code& get_code() const { return m_code; }
		const char* get_description() const;

		// Static
		static ptr create(const http_code& code) { return std::make_shared<http_status>(code); }
		static ptr create(const std::string& str) { return std::make_shared<http_status>(str); }
		static ptr create(const char* psz) { return std::make_shared<http_status>(psz); }
	};
}

#endif
