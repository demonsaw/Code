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

#ifndef _EJA_REQUEST_MESSAGE_H_
#define _EJA_REQUEST_MESSAGE_H_

#include "message/data_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	// Message
	class request_message : public data_message
	{
	protected:
		request_message(const request_message& msg) : data_message(msg) { }
		request_message(const message_type& type) : data_message(type) { }

		// Operator
		request_message& operator=(const request_message& msg);

	public:
		MSGPACK_DEFINE(MSGPACK_BASE(data_message));
	};

	// Operator
	inline request_message& request_message::operator=(const request_message& msg)
	{
		if (this != &msg)
			data_message::operator=(msg);

		return *this;
	}

	// Factory
	template <message_type T>
	class request_message_impl final : public request_message
	{
		make_factory(request_message_impl);

	public:
		request_message_impl() : request_message(T) { }
		request_message_impl(const request_message_impl& msg) : request_message(msg) { }

		// Operator
		request_message_impl& operator=(const request_message_impl& msg);

	public:
		MSGPACK_DEFINE(MSGPACK_BASE(request_message));
	};

	// Operator
	template <message_type T>
	request_message_impl<T>& request_message_impl<T>::operator=(const request_message_impl& msg)
	{
		if (this != &msg)
			request_message::operator=(msg);

		return *this;
	}
}

#endif
