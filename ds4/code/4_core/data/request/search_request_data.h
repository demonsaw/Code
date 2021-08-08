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

#ifndef _EJA_SEARCH_REQUEST_DATA_H_
#define _EJA_SEARCH_REQUEST_DATA_H_

#include <memory>
#include <string>

#include "data/data.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_using_version(search_request_data, 1);

	// Data
	class search_request_data final : public data
	{
		make_factory(search_request_data);

	private:
		std::string m_text;

	public:
		search_request_data() : data(data_type::search_request) { }
		search_request_data(const std::string& text) : data(data_type::search_request), m_text(text) { }

		// Utility
		virtual bool valid() const override { return data::valid() && has_text(); }

		// Has
		bool has_text() const { return !m_text.empty(); }

		// Set
		void set_text(const std::string& text) { m_text = text; }

		// Get
		const std::string& get_text() const { return m_text; }

		// Static
		static ptr create(const std::string& text) { return std::make_shared<search_request_data>(text); }

		MSGPACK_DEFINE(MSGPACK_BASE(data), m_text);
	};
}

#endif
