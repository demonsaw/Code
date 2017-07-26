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
#include <boost/filesystem/path.hpp>

#include "component/service/service_component.h"
#include "message/message_packer.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class folder_component;
	class group_security_list_component;
	class hash;
	class http_request;
	class http_response;
	class http_socket;
	class message_list_component;
	class service_data;
	class share_list_component;
	class transfer_list_component;

	class client_service_component final : public service_component
	{
		make_factory(client_service_component);

	private:
		std::shared_ptr<group_security_list_component> m_group_list;
		std::shared_ptr<message_list_component> m_message_list;
		std::shared_ptr<share_list_component> m_share_list;
		std::shared_ptr<transfer_list_component> m_transfer_list;

	private:
		// Interface
		ptr shared_from_this() { return shared_from_base<client_service_component>(); }

		// Commands
		void file(const std::shared_ptr<entity> data, const std::shared_ptr<folder_component> folder, const std::shared_ptr<entity> client = nullptr);
		
		void async_group();
		void group();

		void async_queue();
		void queue();

		void resume(const std::shared_ptr<entity> data);

		void share();
		void share(const std::shared_ptr<entity> client);

		void unshare();
		void unshare(const std::shared_ptr<entity> client);

	public:
		client_service_component() { }
		client_service_component(const client_service_component& comp) : service_component(comp) { }
		client_service_component(const size_t threads) : service_component(threads) { }

		// Interface
		virtual void update() override;
		virtual void clear() override;

		// Utility
		virtual void start() override;
		virtual void restart() override;
		virtual void stop() override;

		// Operator
		client_service_component& operator=(const client_service_component& comp);

		// Commands
		void async_browse(const std::shared_ptr<entity> data);

		void async_chat(const std::shared_ptr<entity> data, const std::shared_ptr<entity> client = nullptr);

		void async_client();		

		void async_download(const std::shared_ptr<entity> data);

		void async_file(const std::shared_ptr<entity> data, const std::shared_ptr<entity> client = nullptr);
		void async_file(const std::shared_ptr<entity> data, const std::string& path, const std::shared_ptr<entity> client = nullptr);
		void async_file(const std::shared_ptr<entity> data, const boost::filesystem::path& path, const std::shared_ptr<entity> client = nullptr);
		void async_file(const std::shared_ptr<entity> data, const std::shared_ptr<folder_component> path, const std::shared_ptr<entity> client = nullptr);

		void async_folder(const std::shared_ptr<entity> data);
		void async_folder(const std::shared_ptr<entity> data, const std::string& path);
		void async_folder(const std::shared_ptr<entity> data, const boost::filesystem::path& path);

		void async_folder_client(const std::shared_ptr<entity> data);
		void async_folder_client(const std::shared_ptr<entity> data, const std::string& path);
		void async_folder_client(const std::shared_ptr<entity> data, const boost::filesystem::path& path);

		void async_message(); 
		void async_message(const entity::ptr router);

		void async_quit();

		void async_resume(const std::shared_ptr<entity> data);

		void async_search(const std::shared_ptr<entity> data);

		void async_share();
		void async_share(const std::shared_ptr<entity> data);

		void async_transfer(); 
		void async_transfer(const entity::ptr router);

		void async_unshare();
		void async_unshare(const std::shared_ptr<entity> data);

		// Set
		using service_component::set_status;
		virtual void set_status() override;

		// Get
		std::shared_ptr<group_security_list_component> get_group_security_list() const { return m_group_list; }
		std::shared_ptr<message_list_component> get_message_list() const { return m_message_list; }
		std::shared_ptr<share_list_component> get_share_list() const { return m_share_list; }
		std::shared_ptr<transfer_list_component> get_transfer_list() const { return m_transfer_list; }
	};
}

#endif
