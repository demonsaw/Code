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

#ifndef _EJA_ROUTER_ACCEPTOR_COMPONENT_H_
#define _EJA_ROUTER_ACCEPTOR_COMPONENT_H_

#include <memory>
#include <vector>

#include "component/message/message_component.h"
#include "component/service/acceptor_service_component.h"

#include "entity/entity.h"
#include "http/http_code.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_request;
	class http_response;
	class http_socket;
	class message_packer;
	class session_component;

	// State
	enum class router_acceptor_state { none, read, tunnel };

	class router_acceptor_component final : public acceptor_service_component
	{
		make_factory(router_acceptor_component);

	public:
		static const size_t num_threads;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<router_acceptor_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<router_acceptor_component>(shared_from_base<router_acceptor_component>()); }

		// Interface
		virtual void clear() override;

		// Utility
		virtual void close() override;

		void add_messages(const std::shared_ptr<entity>& entity, const std::shared_ptr<message_list_component>& message_list);
		std::shared_ptr<message_list_component> get_messages(const std::shared_ptr<entity>& entity, const size_t overage = 0);

		// Read
		void async_read(const std::shared_ptr<http_socket>& socket) const;
		void read(const std::shared_ptr<http_socket>& socket);

		// Write
		void async_write(const std::shared_ptr<entity>& entity) const;
		void write(const std::shared_ptr<entity>& entity, const std::shared_ptr<message_list_component>& message_list);
		void write(const std::shared_ptr<http_socket>& socket, const http_code code);

		// Commands
		void async_entropy();
		void entropy();

		void async_open();
		void open();

		void async_ping();
		void ping();

		void async_timeout();
		void timeout();

		// Get
		std::shared_ptr<entity> get_entity(const http_request& request) const;
		std::shared_ptr<http_request> get_request(const std::shared_ptr<entity>& entity, const message_packer& packer) const;
		std::shared_ptr<http_response> get_response(const message_packer& packer, const std::shared_ptr<session_component>& session) const;
		std::shared_ptr<http_response> get_response(const http_code code) const;

	public:
		router_acceptor_component() : router_acceptor_component(num_threads) { }
		router_acceptor_component(const size_t threads) : acceptor_service_component(threads) { }

		// Utility
		virtual void start() override;
		virtual void stop() override;

		template <typename T>
		void add_message(const std::shared_ptr<entity>& entity, const T& t);

		// Write
		template <typename T>
		void async_write(const std::shared_ptr<entity>& entity, const T& t);

		// Create
		static ptr create(const size_t threads) { return std::make_shared<router_acceptor_component>(threads); }
	};

	// Utility
	template <typename T>
	void router_acceptor_component::add_message(const std::shared_ptr<entity>& entity, const T& t)
	{
		const auto message_ring = entity->get<message_ring_component>();
		{
			thread_lock(message_ring);
			message_ring->push_back(t);
		}
	}

	// Write
	template <typename T>
	void router_acceptor_component::async_write(const std::shared_ptr<entity>& entity, const T& t)
	{
		const auto self = shared_from_this();

		post([self, entity, t]()
		{
			const auto message_list = self->get_messages(entity, 1);
			message_list->push_back(t);

			self->write(entity, message_list);
		});
	}
}

#endif
