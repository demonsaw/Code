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

#ifndef _EJA_JOIN_REQUEST_MESSAGE_H_
#define _EJA_JOIN_REQUEST_MESSAGE_H_

#include <memory>
#include <string>

#include "message/message_type.h"
#include "message/request/request_message.h"
#include "system/type.h"

// fatal error C1189: #error:  WinSock.h has already been included
#include <msgpack.hpp>

namespace eja
{
	class entity;

	// Version
	make_version(join_request_message, 1);

	// Message
	class join_request_message final : public request_message
	{
		make_factory(join_request_message);

	private:
		std::string m_name;
		std::string m_group;
		u32 m_color = 0;

		bool m_browse = true;
		bool m_chat = true;
		bool m_pm = true;
		bool m_search = true;
		bool m_share = false;
		bool m_upload = true;

	public:
		join_request_message() : request_message(message_type::join_request) { }
		join_request_message(const join_request_message& msg) : request_message(msg), 
			m_name(msg.m_name), m_group(msg.m_group), m_color(msg.m_color),
			m_browse(msg.m_browse), m_chat(msg.m_chat), m_pm(msg.m_pm), m_search(msg.m_search), m_share(msg.m_share), m_upload(msg.m_upload) { }

		// Operator
		join_request_message& operator=(const join_request_message& msg);

		// Has
		bool has_name() const { return !m_name.empty(); }		
		bool has_group() const { return !m_group.empty(); }
		bool has_color() const { return m_color > 0; }

		bool has_browse() const { return m_browse; }
		bool has_chat() const { return m_chat; }
		bool has_pm() const { return m_pm; }
		bool has_search() const { return m_search; }
		bool has_share() const { return m_share; }
		bool has_upload() const { return m_upload; }

		// Set
		void set_name(const std::string& name) { m_name = name; }		
		void set_group(const std::string& group) { m_group = group; }
		
		void set_color(const u8 red, const u8 green, const u8 blue, const u8 alpha = 255) { m_color = (alpha << 24) + (red << 16) + (green << 8) + (blue << 0); }
		void set_color(const u32 color) { m_color = color; }

		void set_browse(const bool browse) { m_browse = browse; }
		void set_chat(const bool chat) { m_chat = chat; }
		void set_pm(const bool pm) { m_pm = pm; }
		void set_search(const bool search) { m_search = search; }		
		void set_share(const bool share) { m_share = share; }
		void set_upload(const bool upload) { m_upload = upload; }

		// Get
		const std::string& get_name() const { return m_name; }		
		const std::string& get_group() const { return m_group; }
		u32 get_color() const { return m_color; }

		// Static
		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const T& data) { return ptr(new join_request_message(entity, data)); }

		template <typename T>
		static ptr create(const std::shared_ptr<entity> entity, const std::shared_ptr<T> data) { return ptr(new join_request_message(entity, *data)); }

		MSGPACK_DEFINE(MSGPACK_BASE(request_message), m_name, m_group, m_color, m_browse, m_chat, m_pm, m_search, m_share, m_upload);
	};

	// Operator
	inline join_request_message& join_request_message::operator=(const join_request_message& msg)
	{
		if (this != &msg)
		{
			request_message::operator=(msg);

			m_name = msg.m_name;
			m_group = msg.m_group;
			m_color = msg.m_color;

			m_browse = msg.m_browse;
			m_chat = msg.m_chat;
			m_pm = msg.m_pm;
			m_search = msg.m_search;
			m_share = msg.m_share;
			m_upload = msg.m_upload;
		}

		return *this;
	}
}

#endif
