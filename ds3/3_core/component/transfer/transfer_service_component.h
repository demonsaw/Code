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

#ifndef _EJA_TRANSFER_SERVICE_COMPONENT_H_
#define _EJA_TRANSFER_SERVICE_COMPONENT_H_

#include <list>
#include <memory>

#include "component/service/service_component.h"
#include "message/message_packer.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;
	class group_security_list_component;
	class http_request;
	class http_response;
	class http_socket;
	class service_data;

	class transfer_service_component : public service_component
	{
		make_factory(transfer_service_component);

	private:
		enum class async_state { none, handshake };

		std::shared_ptr<group_security_list_component> m_group_list;
		eja::thread_safe<std::list<std::shared_ptr<message_packer>>> m_packer_list;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<transfer_service_component>(); }

		// Utility
		virtual void close() override;
		
		void pause(const std::shared_ptr<message_packer> packer);
		void resume(const std::shared_ptr<service_data> data = nullptr);

		// Commands
		void download(const std::shared_ptr<entity> data);

		void async_group();
		void group();

		void async_handshake();
		void handshake();

		void quit();

		void upload(const std::shared_ptr<entity> data);

		// Request
		template <typename T>
		void write_request(const T t, const std::shared_ptr<service_data> data = nullptr);
		void write_request(const std::shared_ptr<message_packer> packer, const std::shared_ptr<service_data> data = nullptr);

		template <typename T>
		void write_request(const std::shared_ptr<http_socket> socket, const T t, const std::shared_ptr<service_data> data = nullptr);
		void write_request(const std::shared_ptr<http_socket> socket, const std::shared_ptr<message_packer> packer, const std::shared_ptr<service_data> data = nullptr);

		// Response
		void read_response(const std::shared_ptr<http_socket> socket, const std::shared_ptr<message_packer> packer, const std::shared_ptr<service_data> data = nullptr);
		void read_response(const std::shared_ptr<http_response> response, const std::shared_ptr<service_data> data = nullptr);

	public:
		transfer_service_component();
		transfer_service_component(const transfer_service_component& comp) : service_component(comp) { }
		transfer_service_component(const size_t threads) : service_component(threads) { }

		// Interface
		virtual void update() override;
		virtual void clear() override;

		// Utility
		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;

		// Operator
		transfer_service_component& operator=(const transfer_service_component& comp);

		// Commands
		void async_download(const std::shared_ptr<entity> data);
		void async_quit();
		void async_upload(const std::shared_ptr<entity> data);

		// Get
		std::shared_ptr<group_security_list_component> get_group_security_list() const { return m_group_list; }
	};

	// Request
	template <typename T>
	void transfer_service_component::write_request(const T t, const std::shared_ptr<service_data> data /*= nullptr*/)
	{
		const auto packer = message_packer::create();
		packer->push_back(t);

		write_request(packer, data);
	}

	template <typename T>
	void transfer_service_component::write_request(const std::shared_ptr<http_socket> socket, const T t, const std::shared_ptr<service_data> data /*= nullptr*/)
	{
		const auto packer = message_packer::create();
		packer->push_back(t);

		write_request(socket, packer, data);
	}
}

#endif
