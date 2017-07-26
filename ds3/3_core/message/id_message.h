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

#ifndef _EJA_ID_MESSAGE_H_
#define _EJA_ID_MESSAGE_H_

#include <memory>
#include <string>

#include "message/message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Message
	class id_message : public message
	{
	protected:
		std::string m_id;

	protected:
		id_message(const id_message& msg) : message(msg), m_id(msg.m_id) { }
		id_message(const message_type& type) : message(type) { }

		// Operator
		id_message& operator=(const id_message& msg);

	public:
		// Has
		bool has_id() const { return !m_id.empty(); }

		// Set
		void set_id() { m_id.clear(); }
		void set_id(std::string&& id) { m_id = std::move(id); }
		void set_id(const std::string& id) { m_id = id; }
		void set_id(const void* id, const size_t size) { m_id.assign(reinterpret_cast<const char*>(id), size); }
		void set_id(const char* id) { m_id.assign(id, strlen(id)); }

		// Get
		const std::string& get_id() const { return m_id; }

		MSGPACK_DEFINE(m_id);
	};

	// Operator
	inline id_message& id_message::operator=(const id_message& msg)
	{
		if (this != &msg)
		{
			message::operator=(msg);

			m_id = msg.m_id;
		}

		return *this;
	}
}

#endif
