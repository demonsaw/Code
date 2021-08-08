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

#ifndef _EJA_ROUTER_SERVICE_COMPONENT_H_
#define _EJA_ROUTER_SERVICE_COMPONENT_H_

#include <memory>
#include <string>

#include "component/client/client_component.h"
#include "component/message/message_component.h"
#include "component/service/socket_service_component.h"

#include "entity/entity.h"
#include "http/http_code.h"
#include "message/message_packer.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_request;
	class http_response;
	class http_socket;
	class share_list_component;

	enum class router_service_state { none, handshake, join, tunnel };

	make_thread_safe_type(http_socket_set, std::unordered_set<std::shared_ptr<http_socket>>);

	class router_service_component : public socket_service_component
	{
		make_factory(router_service_component);

	private:
		http_socket_set::ptr m_cache;
		std::shared_ptr<http_socket> m_tunnel;
		std::shared_ptr<share_list_component> m_share_list;

	public:
		static const size_t min_threads;
		static const size_t num_threads;
		static const size_t max_threads;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<router_service_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<router_service_component>(shared_from_base<router_service_component>()); }

		// Utility
		virtual void close() override;

		void async_restart();

		void add_cache(const std::shared_ptr<http_socket>& socket);
		void remove_cache(const std::shared_ptr<http_socket>& socket);

		template <typename T>
		void add_message(const T& t);
		void add_messages(const std::shared_ptr<message_list_component>& message_list);
		std::shared_ptr<message_list_component> get_messages(const size_t overage = 0);

		// Commands
		void async_handshake();
		void handshake();

		void async_join();
		void join();

		void async_ping();
		void ping();

		void async_tunnel();
		void tunnel();

		// Read
		void async_read(const std::shared_ptr<http_socket>& socket);
		void read(const std::shared_ptr<http_socket>& socket);

		// Write
		void async_write();

		void write();
		void write(const message_packer& packer, const router_service_state state);
		void write(const message_packer& packer, const std::shared_ptr<message_list_component>& message_list);

		template <typename T>
		void write(const T& t);

		template <typename T>
		void write(const T& t, const router_service_state state);

		// Execute
		void async_parse(const std::shared_ptr<http_request>& request);
		void parse(const std::shared_ptr<http_request>& request);
		void parse(const std::shared_ptr<http_response>& response);
		void parse(const std::string& str);

		// Set
		virtual void set_status(const std::shared_ptr<entity>& entity, const status value) const override;

		// Get
		template <typename T>
		std::shared_ptr<http_request> get_request(const T& t) const;
		std::shared_ptr<http_request> get_request(const message_packer& packer) const;

		std::shared_ptr<http_response> get_response(const message_packer& packer) const;
		std::shared_ptr<http_response> get_response(const http_code code) const;

	public:
		router_service_component() : router_service_component(max_threads) { }
		router_service_component(const size_t threads) : socket_service_component(threads), m_cache(std::make_shared<http_socket_set>()) { }

		// Interface
		virtual void clear() override;

		// Utility
		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;

		// Commands
		void async_quit();

		template <typename T>
		void async_write(const T& t);

		// Set
		void set_share_list(const std::shared_ptr<share_list_component>& share_list) { m_share_list = share_list; }

		// Create
		static ptr create(const size_t threads) { return std::make_shared<router_service_component>(threads); }
	};

	// Utility
	template <typename T>
	void router_service_component::add_message(const T& t)
	{
		const auto owner = get_owner();
		const auto message_ring = owner->get<message_ring_component>();
		{
			thread_lock(message_ring);
			message_ring->push_back(t);
		}
	}

	// Write
	template <typename T>
	void router_service_component::async_write(const T& t)
	{
		// NOTE: Wait until we have joined the router
		const auto owner = get_owner();
		const auto client = owner->get<client_component>();

		if (client->has_id())
		{
			const auto self = shared_from_this();
			post([self, t]() { self->write(t); });
		}
	}

	template <typename T>
	void router_service_component::write(const T& t)
	{
		const auto message_list = get_messages(1);
		message_list->push_back(t);

		message_packer packer;
		packer.insert(packer.end(), message_list->begin(), message_list->end());

		write(packer, message_list);
	}

	template <typename T>
	void router_service_component::write(const T& t, const router_service_state state)
	{
		message_packer packer(t);

		write(packer, state);
	}

	// Get
	template <typename T>
	std::shared_ptr<http_request> router_service_component::get_request(const T& t) const
	{
		message_packer packer(t);

		return get_request(packer);
	}
}

#endif
