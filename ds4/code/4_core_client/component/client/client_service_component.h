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

#ifndef _EJA_CLIENT_SERVICE_COMPONENT_H_
#define _EJA_CLIENT_SERVICE_COMPONENT_H_

#include <memory>
#include <thread>
#include <boost/filesystem/path.hpp>

#include "component/router/router_service_component.h"
#include "component/service/socket_service_component.h"
#include "message/message_packer.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class group_list_component;
	class mute_request_data;
	class room_request_data;
	class share_list_component;

	class client_service_component final : public socket_service_component
	{
		make_factory(client_service_component);

	private:
		std::shared_ptr<group_list_component> m_group_list;
		std::shared_ptr<share_list_component> m_share_list;

	public:
		static const size_t min_threads;
		static const size_t max_threads;
		static const size_t num_threads;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<client_service_component>(); }
		const ptr shared_from_this() const { return std::const_pointer_cast<client_service_component>(shared_from_base<client_service_component>()); }

		// Commands
		void async_entropy();
		void entropy();

		void async_mute(const std::shared_ptr<entity>& entity, const std::shared_ptr<mute_request_data>& request_data) const;

		void async_room(const std::shared_ptr<room_request_data>& request_data);

		void async_router();
		void router();

		void async_timeout();
		void timeout();

		void share();
		void share(const std::shared_ptr<entity>& entity);

		void unshare();
		void unshare(const std::shared_ptr<entity>& entity);

		// Set
		virtual void set_status(const std::shared_ptr<entity>& entity, const status value) const override;

		void set_group_list();
		void set_share_list();

	public:
		client_service_component() : client_service_component(max_threads) { }
		client_service_component(const size_t threads);

		// Interface
		virtual void clear() override;

		// Utility
		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;

		// Commands
		void async_browse(const std::shared_ptr<entity>& entity);

		void async_chat(const std::string& text);
		void async_chat(const std::shared_ptr<entity>& entity, const std::string& text);
		void async_chat(const std::string& room, const std::string& text);

		void async_download(const std::shared_ptr<entity>& entity);

		void async_join();

		void async_mute(const std::shared_ptr<entity>& entity, const bool value) const;

		void async_room();
		void async_room_browse(const std::shared_ptr<entity>& entity);
		void async_room_join(const std::shared_ptr<entity>& entity);
		void async_room_quit(const std::shared_ptr<entity>& entity);

		void async_search(const std::shared_ptr<entity>& entity);

		void async_share();
		void async_share(const std::shared_ptr<entity>& entity);

		void async_unshare();
		void async_unshare(const std::shared_ptr<entity>& entity);

		template <typename T>
		void async_write(const T& t);

		// Create
		static ptr create(const size_t threads) { return std::make_shared<client_service_component>(threads); }
	};

	// Write
	template <typename T>
	void client_service_component::async_write(const T& t)
	{
		// Write
		const auto owner = get_owner();
		const auto router_service = owner->get<router_service_component>();
		router_service->async_write(t);
	}
}

#endif
