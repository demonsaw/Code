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

#ifndef _EJA_DATA_MESSAGE_H_
#define _EJA_DATA_MESSAGE_H_

#include <memory>
#include <string>

#include "message/id_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Version
	make_version(data_message, 1);

	// Message
	class data_message : public id_message
	{
	protected:
		std::string m_client;
		std::string m_data;

	protected:
		data_message(const data_message& msg) : id_message(msg), m_client(msg.m_client), m_data(msg.m_data) { }
		data_message(const message_type& type) : id_message(type) { }

		// Operator
		data_message& operator=(const data_message& msg);

	public:		
		// Interface
		virtual void clear() { m_data.clear(); }

		// Has
		bool has_client() const { return !m_client.empty(); }
		bool has_data() const { return !m_data.empty(); }

		// Set
		void set_client() { m_client.clear(); }
		void set_client(const std::string& client) { m_client = client; }

		void set_data() { m_data.clear(); }
		void set_data(std::string&& data) { m_data = std::move(data); }
		void set_data(const std::string& data) { m_data = data; }
		void set_data(const void* data, const size_t size) { m_data.assign(reinterpret_cast<const char*>(data), size); }
		void set_data(const char* data) { m_data.assign(data, strlen(data)); }

		// Get
		const std::string& get_client() const { return m_client; }
		const std::string& get_data() const { return m_data; }
		std::string& get_data() { return m_data; }

		MSGPACK_DEFINE(MSGPACK_BASE(id_message), m_client, m_data);
	};

	// Operator
	inline data_message& data_message::operator=(const data_message& msg)
	{
		if (this != &msg)
		{
			id_message::operator=(msg);

			m_client = msg.m_client;
			m_data = msg.m_data;
		}

		return *this;
	}
}

#endif
