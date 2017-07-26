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

#ifndef _EJA_JSON_COMMAND_
#define _EJA_JSON_COMMAND_

#include <iostream>
#include <memory>
#include <thread>

#include "json_data.h"
#include "json_message.h"
#include "json_packet.h"
#include "json_type.h"
#include "component/function_component.h"
#include "component/error/error_component.h"
#include "entity/entity_command.h"
#include "http/http_socket.h"
#include "http/http_status.h"
#include "system/function/function_type.h"
#include "system/function/function_action.h"
#include "utility/default_value.h"

namespace eja
{
	class json_async_command;

	class json_command : public entity_command, public std::enable_shared_from_this<json_command>
	{
	protected:
		http_socket::ptr m_socket = nullptr;
		std::string m_id;

	public:
		using ptr = std::shared_ptr<json_command>;

	protected:
		json_command(const json_command&) = delete;
		json_command(const entity::ptr entity) : entity_command(entity) { }
		json_command(const entity::ptr entity, const http_socket::ptr socket) : entity_command(entity), m_socket(socket) { }
		virtual ~json_command() override { }

		// Operator
		json_command& operator=(const json_command&) = delete;

		// Utility		
		void call(const function_type type, const function_action action, const entity::ptr entity = nullptr) const { m_entity->call(type, action, entity); }

		// Component
		function_map_list_component::ptr get_function_map() const { return m_entity->get<function_map_list_component>(); }
		error_component::ptr get_error() const { return m_entity->get<error_component>(); }

	public:
		// Interface
		virtual http_status execute() { return http_code::none; }
		virtual http_status execute(const json_packet::ptr packet) { return http_code::none; }
		virtual http_status execute(const json_message::ptr message) { return http_code::none; }
		virtual http_status execute(const json_message::ptr message, const json_data::ptr data) { return http_code::none; }
		virtual http_status execute(const json_data::ptr data) { return http_code::none; }

		virtual http_status execute(const entity::ptr entity) { return http_code::none; }
		virtual http_status execute(const entity::ptr entity, const json_packet::ptr packet) { return http_code::none; }
		virtual http_status execute(const entity::ptr entity, const json_message::ptr message) { return http_code::none; }
		virtual http_status execute(const entity::ptr entity, const json_message::ptr message, const json_data::ptr data) { return http_code::none; }
		virtual http_status execute(const entity::ptr entity, const json_data::ptr data) { return http_code::none; }

		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return entity_command::valid() && m_socket->valid(); }

		std::shared_ptr<json_async_command> make_async() const;
		bool has_socket() const { return m_socket != nullptr; }
		
		void log(const std::exception& e);
		void log(const std::string& str);
		void log(const char* psz = default_error::unknown);

		// Mutator
		void set_socket(const http_socket::ptr socket) { m_socket = socket; }
		void set_id(const std::string& id) { m_id = id; }

		// Accessor
		const http_socket::ptr get_socket() const { return m_socket; }
		const std::string& get_id() const { return m_id; }
	};

	class json_async_command final : public entity_command, public std::enable_shared_from_this<json_async_command>
	{
		friend class json_command;

	private:
		json_command::ptr m_command;

	public:
		using ptr = std::shared_ptr<json_async_command>;

	private:
		json_async_command(const json_command::ptr command) : m_command(command) { }
		json_async_command(const json_async_command&) = delete;

		// Operator
		json_async_command& operator=(const json_async_command&) = delete;

	public:		
		virtual ~json_async_command() { }

		// Interface
		virtual void clear() override;

		// Command
		void execute(const entity::ptr entity);
		void execute(const entity::ptr entity, const json_message::ptr message);
		void execute(const entity::ptr entity, const json_data::ptr data);		

		// Accessor
		const json_command::ptr get_command() const { return m_command; }

		// Static
		template <typename T>
		static ptr create(const entity::ptr entity, const http_socket::ptr socket) { return ptr(new json_async_command(T::create(entity, socket))); }
		static ptr create(const json_command::ptr command) { return ptr(new json_async_command(command)); }
	};
}

#endif