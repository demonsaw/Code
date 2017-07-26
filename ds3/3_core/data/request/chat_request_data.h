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

#ifndef _EJA_CHAT_REQUEST_DATA_H_
#define _EJA_CHAT_REQUEST_DATA_H_

#include "data/data.h"
#include "data/data_type.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(chat_request_data, 1);

	// Data
	class chat_request_data final : public data
	{
		make_factory(chat_request_data);

	private:		
		std::string m_text;
		bool m_pm = false;

	public:
		chat_request_data() : data(data_type::chat_request) { }
		chat_request_data(const chat_request_data& dat) : data(dat), m_pm(dat.m_pm), m_text(dat.m_text) { }

		// Operator
		chat_request_data& operator=(const chat_request_data& dat);

		// Utility
		virtual bool valid() const { return has_text(); }

		// Has
		bool has_pm() const { return m_pm; }
		bool has_text() const { return !m_text.empty(); }

		// Set
		void set_pm(const bool pm) { m_pm = pm; }
		void set_text(const std::string& text) { m_text = text; }

		// Get
		const std::string& get_text() const { return m_text; }

		MSGPACK_DEFINE(m_pm, m_text);
	};

	// Operator
	inline chat_request_data& chat_request_data::operator=(const chat_request_data& dat)
	{
		if (this != &dat)
		{
			data::operator=(dat);

			m_pm = dat.m_pm;
			m_text = dat.m_text;
		}

		return *this;
	}
}

#endif
