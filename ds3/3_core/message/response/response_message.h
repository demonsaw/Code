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

#ifndef _EJA_RESPONSE_MESSAGE_H_
#define _EJA_RESPONSE_MESSAGE_H_

#include "message/data_message.h"
#include "message/message_status.h"

namespace eja
{
	class response_message : public data_message
	{
	protected:
		message_status m_status = message_status::none;

	protected:
		response_message(const response_message& msg) : data_message(msg) { }
		response_message(const message_type& type) : data_message(type) { }

		// Operator
		response_message& operator=(const response_message& msg);

	public:
		// Utility
		bool success() const { return m_status == message_status::success; }
		bool error() const { return m_status == message_status::error; }

		// Has
		bool has_status() const { return m_status != message_status::none; }

		// Set
		void set_status() { m_status = message_status::none; }
		void set_status(const message_status& status) { m_status = status; }

		void set_success() { set_status(message_status::success); }
		void set_error() { set_status(message_status::error); }

		// Get
		message_status get_status() const { return m_status; }

		MSGPACK_DEFINE(MSGPACK_BASE(data_message), m_status);
	};

	// Operator
	inline response_message& response_message::operator=(const response_message& msg)
	{
		if (this != &msg)
			data_message::operator=(msg);

		return *this;
	}

	// Factory
	template <message_type T>
	class response_message_impl final : public response_message
	{
		make_factory(response_message_impl);

	public:
		response_message_impl() : response_message(T) { }
		response_message_impl(const response_message_impl& msg) : response_message(msg) { }

		// Operator
		response_message_impl& operator=(const response_message_impl& msg);

		MSGPACK_DEFINE(MSGPACK_BASE(response_message));
	};

	// Operator
	template <message_type T>
	response_message_impl<T>& response_message_impl<T>::operator=(const response_message_impl& msg)
	{
		if (this != &msg)
			response_message::operator=(msg);

		return *this;
	}
}

#endif
