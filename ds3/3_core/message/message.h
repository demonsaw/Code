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

#ifndef _EJA_MESSAGE_H_
#define _EJA_MESSAGE_H_

#include <list>
#include <memory>

#include "message/message_type.h"
#include "object/object.h"

namespace eja
{
	class message;
	using message_list = std::list<std::shared_ptr<message>>;

	class message : public object
	{
	protected:		
		message_type m_type = message_type::none;

	protected:
		message() { }
		message(const message& msg) : object(msg), m_type(msg.m_type) { }
		message(const message_type& type) : m_type(type) { }

		// Operator
		message& operator=(const message& msg);

	public:
		// Has		
		bool has_type() const { return m_type != message_type::none; }

		// Set
		void set_type(const message_type& type) { m_type = type; }

		// Get		
		message_type get_type() const { return m_type; }
	};

	// Operator
	inline message& message::operator=(const message& msg)
	{
		if (this != &msg)
		{
			object::operator=(msg);
			
			m_type = msg.m_type;
		}

		return *this;
	}
}

#endif
