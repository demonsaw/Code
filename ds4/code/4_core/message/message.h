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

#include <deque>
#include <memory>
#include <string>

#include "message/message_type.h"
#include "object/mode.h"
#include "object/object.h"

namespace eja
{
	class message;

	using message_list = std::deque<std::shared_ptr<message>>;

	class message : public object, public mode<u8>
	{
	protected:
		std::string m_id;
		std::string m_origin;
		string_deque m_clients;
		string_ptr m_data;
		message_type m_type = message_type::none;

	public:
		using ptr = std::shared_ptr<message>;

	protected:
		message() { }
		explicit message(const message& msg) : object(msg), mode<u8>(msg), m_id(msg.m_id), m_clients(msg.m_clients), m_origin(msg.m_origin), m_data(msg.m_data), m_type(msg.m_type) { }
		explicit message(const message_type& type) : m_type(type) { }
		explicit message(const message_type& type, const u8 bits) : mode<u8>(bits), m_type(type) { }
		explicit message(const u8 bits) : mode<u8>(bits) { }

		// Operator
		message& operator=(const message& msg);

	public:
		// Interface
		virtual void clear() override;

		// Utility
		void add(const std::string& client) { m_clients.push_back(client); }
		void add(const string_deque& clients) { m_clients.insert(m_clients.end(), clients.begin(), clients.end()); }

		// Has
		bool has_id() const { return !m_id.empty(); }
		bool has_origin() const { return !m_origin.empty(); }
		bool has_data() const { return m_data != nullptr; }
		bool has_type() const { return m_type != message_type::none; }

		bool has_client() const { return m_clients.size() == 1; }
		bool has_clients() const { return !m_clients.empty(); }

		// Set
		void set_id() { m_id.clear(); }
		void set_id(std::string&& id) { m_id = std::move(id); }
		void set_id(const std::string& id) { m_id = id; }
		void set_id(const void* id, const size_t size) { m_id.assign(reinterpret_cast<const char*>(id), size); }
		void set_id(const char* id) { m_id.assign(id, strlen(id)); }

		void set_client() { m_clients.clear(); }
		void set_client(const std::string& client) { m_clients.assign(1, client); }

		void set_clients() { m_clients.clear(); }
		void set_clients(const string_deque& clients) { m_clients = clients; }

		void set_origin() { m_origin.clear(); }
		void set_origin(std::string&& origin) { m_origin = std::move(origin); }
		void set_origin(const std::string& origin) { m_origin = origin; }
		void set_origin(const void* origin, const size_t size) { m_origin.assign(reinterpret_cast<const char*>(origin), size); }
		void set_origin(const char* origin) { m_origin.assign(origin, strlen(origin)); }

		void set_data() { m_data.reset(); }
		void set_data(const string_ptr& ptr) { m_data = ptr; }
		void set_data(const void* data, const size_t size) { m_data = std::make_shared<std::string>(reinterpret_cast<const char*>(data), size); }
		void set_data(std::string&& data) { m_data = std::make_shared<std::string>(std::move(data)); }
		void set_data(const std::string& str) { m_data = std::make_shared<std::string>(str); }
		void set_data(const char* psz) { m_data = std::make_shared<std::string>(psz); }

		void set_type(const message_type type) { m_type = type; }

		// Get
		const std::string& get_id() const { return m_id; }
		const std::string& get_origin() const { return m_origin; }
		const string_ptr& get_data() const { return m_data; }
		message_type get_type() const { return m_type; }

		const std::string& get_client() const { return m_clients.at(0); }
		const string_deque& get_clients() const { return m_clients; }

		MSGPACK_DEFINE(m_mode, m_id, m_clients, m_origin, m_data);
	};

	// Operator
	inline message& message::operator=(const message& msg)
	{
		if (this != &msg)
		{
			object::operator=(msg);
			mode<u8>::operator=(msg);

			m_id = msg.m_id;
			m_clients = msg.m_clients;
			m_origin = msg.m_origin;
			m_data = msg.m_data;
			m_type = msg.m_type;
		}

		return *this;
	}

	// Factory
	template <message_type T>
	class message_impl final : public message
	{
		make_factory(message_impl);

	public:
		message_impl() : message(T) { }

		MSGPACK_DEFINE(MSGPACK_BASE(message));
	};
}

#endif
