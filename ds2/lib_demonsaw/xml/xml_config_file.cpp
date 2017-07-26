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

#if ANDROID
#include <stdio.h>
#endif

#include <iostream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>

#include "xml.h"
#include "xml_config_file.h"
#include "component/session_component.h"
#include "component/share_idle_component.h"
#include "component/timer_component.h"
#include "component/client/client_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_security_component.h"
#include "component/error/error_component.h"
#include "component/group/group_component.h"
#include "component/group/group_security_component.h"
#include "component/group/group_option_component.h"
//#include "component/io/file_component.h"
#include "component/router/router_component.h"
#include "component/router/router_machine_component.h"
#include "component/router/router_option_component.h"
#include "component/router/router_security_component.h"
#include "component/server/server_component.h"
#include "component/server/server_machine_component.h"
#include "component/server/server_option_component.h"
#include "component/server/server_security_component.h"
#include "component/transfer/chunk_component.h"

#include "entity/entity.h"
#include "utility/boost.h"
#include "utility/default_value.h"
#include "utility/std.h"
#include "utility/client/client_util.h"
#include "utility/group/group_util.h"
#include "utility/router/router_util.h"
#include "utility/server/server_util.h"

namespace eja
{
	// Interface
	void xml_config_file::clear()
	{
		//xml_file::clear();

		boost::optional<boost::property_tree::iptree&> pt_root = m_tree.get_child_optional(xml::demonsaw);
		if (pt_root)
			pt_root->clear();
		
		//m_clients.clear();
		//m_routers.clear();
		m_version = default_version::xml;
	}

	bool xml_config_file::read(const char* path)
	{
		xml_file::read(path);

		// Root
		const auto& pt_root = m_tree.get_child(xml::demonsaw, empty_ptree());
		if (pt_root.empty())
		{
			boost::property_tree::iptree pt_root;
			m_tree.add_child(xml::demonsaw, pt_root);
			return false;
		}

		// Attribute
		const auto& pt_attribute = pt_root.get_child(xml::attribute, empty_ptree());
		if (!pt_attribute.empty())
			m_version = pt_attribute.get<size_t>(xml::version, default_version::xml);

		// Client
		auto pt_clients = pt_root.get_child(xml::clients, empty_ptree());

		for (const auto& pair : pt_clients)
		{
			try
			{
				const auto pt_client = pair.second;
				if (pt_client.empty())
					continue;

				// Entity
				const auto entity = client_util::create();

				get_client(pt_client, entity);
				get_client_router(pt_client, entity);
				get_client_security(pt_client, entity);
				get_client_option(pt_client, entity);
				get_client_group(pt_client, entity);
				get_client_folder(pt_client, entity);
				get_client_file(pt_client, entity);

				// TODO
				//
				//
				//get_client_server_vector(pt_client, entity);
				m_clients.add(entity);
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
			catch (...)
			{
				std::cerr << "Unable to load client" << std::endl;
			}

			// Limit the total number of clients per process
			if (m_clients.size() >= default_client::max_entities)
				break;
		}

		// Router
		auto pt_routers = pt_root.get_child(xml::routers, empty_ptree());

		for (const auto& pair : pt_routers)
		{
			try
			{
				const auto pt_router = pair.second;
				if (pt_router.empty())
					continue;

				// Entity
				const auto entity = router_util::create();
				get_router(pt_router, entity);
				get_router_option(pt_router, entity);
				get_router_servers(pt_router, entity);
				m_routers.add(entity);
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
			catch (...)
			{
				std::cerr << "Unable to load router" << std::endl;
			}

			// Limit the total number of router per process
			if (m_routers.size() >= default_router::max_entities)
				break;
		}

		return true;
	}

	bool xml_config_file::write(const char* path)
	{
		// Root
		auto& pt_root = m_tree.get_child(xml::demonsaw);

		// Attribute
		boost::property_tree::iptree pt_attribute;
		pt_attribute.put<size_t>(xml::version, m_version);
		pt_root.add_child(xml::attribute, pt_attribute);

		{
			// Client
			boost::property_tree::iptree pt_clients;
			if (!m_clients.empty())
			{
				auto_lock_ref(m_clients);
				for (const auto& entity : m_clients)
				{
					boost::property_tree::iptree pt_client;

					// Attribute
					boost::property_tree::iptree pt_attribute;
					pt_client.add_child(xml::attribute, pt_attribute);

					// Entity
					put_client(pt_client, entity);
					put_client_router(pt_client, entity);
					put_client_security(pt_client, entity);
					put_client_option(pt_client, entity);

					put_client_group(pt_client, entity);
					put_client_folder(pt_client, entity);
					put_client_file(pt_client, entity);

					pt_clients.add_child(xml::client, pt_client);
				}

				pt_root.put_child(xml::clients, pt_clients);
			}			
		}

		{
			// Router
			boost::property_tree::iptree pt_routers;
			if (!m_routers.empty())
			{
				auto_lock_ref(m_routers);
				for (const auto& entity : m_routers)
				{
					boost::property_tree::iptree pt_router;

					// Attribute
					boost::property_tree::iptree pt_attribute;
					pt_router.add_child(xml::attribute, pt_attribute);

					// Entity
					put_router(pt_router, entity);
					put_router_option(pt_router, entity);
					put_router_servers(pt_router, entity);

					pt_routers.add_child(xml::router, pt_router);
				}

				pt_root.put_child(xml::routers, pt_routers);
			}			
		}

		return xml_file::write(path);
	}

	// Mutator
	void xml_config_file::put_client(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto client = entity->get<client_component>();

		tree.put(xml::name, client->get_name());
	}

	void xml_config_file::put_client_file(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto share = entity->get<share_idle_component>();
		boost::property_tree::iptree pt_files;

		const auto files = share->get_files();
		if (!files.empty())
		{
			auto_lock_ref(files);
			for (const auto& file : files)
				pt_files.add(xml::file, file->get_path());

			tree.put_child(xml::files, pt_files);
		}		
	}

	void xml_config_file::put_client_folder(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto share = entity->get<share_idle_component>();
		boost::property_tree::iptree pt_folders;

		const auto folders = share->get_folders();
		if (!folders.empty())
		{
			auto_lock_ref(folders);
			for (const auto& folder : folders)
				pt_folders.add(xml::folder, folder->get_path());

			tree.put_child(xml::folders, pt_folders);
		}		
	}

	void xml_config_file::put_client_group(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		boost::property_tree::iptree pt_groups;

		const auto group_vector = entity->get<group_entity_vector_component>();
		if (!group_vector)
			return;

		auto_lock_ptr(group_vector);

		for (const auto& e : *group_vector)
		{
			boost::property_tree::iptree pt_group;

			// Option			
			boost::property_tree::iptree pt_attribute;
			const auto option = e->get<group_option_component>();
			pt_attribute.put(xml::enabled, option->enabled());
			pt_group.add_child(xml::attribute, pt_attribute);

			// Security
			const auto file = e->get<file_component>();
			pt_group.add(xml::path, file->get_path());

			const auto security = e->get<group_security_component>();
			const auto entropy = boost::str(boost::format("%.02f") % security->get_entropy());
			pt_group.add(xml::entropy, entropy);

			pt_group.add(xml::cipher_type, security->get_cipher_type());
			pt_group.add(xml::key_size, security->get_key_size());
			pt_group.add(xml::hash_type, security->get_hash_type());
			pt_group.add(xml::salt, security->get_salt());
			pt_group.add(xml::iterations, security->get_iterations());

			pt_groups.add_child(xml::group, pt_group);
		}

		tree.put_child(xml::groups, pt_groups);
	}

	void xml_config_file::put_client_option(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto option = entity->get<client_option_component>();

		// Attribute
		auto& pt_attribute = tree.get_child(xml::attribute);
		pt_attribute.put(xml::enabled, option->enabled());

		// Timeout
		boost::property_tree::iptree pt_timeout;
		pt_timeout.put(xml::socket, option->get_socket_timeout());
		pt_timeout.put(xml::download, option->get_download_timeout());
		pt_timeout.put(xml::upload, option->get_upload_timeout());
		pt_timeout.put(xml::ping, option->get_ping_timeout());
		tree.put_child(xml::timeout, pt_timeout);

		// Max
		boost::property_tree::iptree pt_max;
		pt_max.put(xml::errors, option->get_max_errors());
		tree.put_child(xml::max, pt_max);

		// Transfer
		boost::property_tree::iptree pt_transfer;
		pt_transfer.put(xml::path, option->get_download_path());
		pt_transfer.put(xml::partial, option->has_partial());
		pt_transfer.put(xml::searches, option->get_max_searches());
		pt_transfer.put(xml::downloads, option->get_max_downloads());
		pt_transfer.put(xml::uploads, option->get_max_uploads());
		pt_transfer.put(xml::retries, option->get_max_retries());
		tree.put_child(xml::transfer, pt_transfer);

		// Priority
		boost::property_tree::iptree pt_priority;
		pt_priority.put(xml::size, b_to_mb(option->get_priority_size()));
		pt_priority.put(xml::extension, option->get_priority_ext());
		tree.put_child(xml::priority, pt_priority);

		// Threads
		boost::property_tree::iptree pt_thread;
		pt_thread.put(xml::downloads, option->get_download_threads());
		pt_thread.put(xml::uploads, option->get_upload_threads());
		pt_thread.put(xml::size, b_to_mb(option->get_thread_size()));
		tree.put_child(xml::thread, pt_thread);

		// Chat
		boost::property_tree::iptree pt_chat;
		pt_chat.put(xml::audio, option->has_chat_audio());
		pt_chat.put(xml::visual, option->has_chat_visual());
		pt_chat.put(xml::sound, option->get_chat_sound());
		pt_chat.put(xml::timestamp, option->get_chat_timestamp());
		pt_chat.put(xml::volume, option->get_chat_volume());
		pt_chat.put(xml::history, option->get_chat_history());
		tree.put_child(xml::chat, pt_chat);

		// Message
		boost::property_tree::iptree pt_message;
		pt_message.put(xml::audio, option->has_message_audio());
		pt_message.put(xml::visual, option->has_message_visual());
		pt_message.put(xml::sound, option->get_message_sound());
		pt_message.put(xml::timestamp, option->get_message_timestamp());
		pt_message.put(xml::volume, option->get_message_volume());
		tree.put_child(xml::message, pt_message);

		// Action
		boost::property_tree::iptree pt_action;
		pt_action.put(xml::browse, option->get_browse());
		pt_action.put(xml::search, option->get_search());
		pt_action.put(xml::transfer, option->get_transfer());
		pt_action.put(xml::chat, option->get_chat());
		pt_action.put(xml::message, option->get_message());
		tree.put_child(xml::action, pt_action);
	}

	void xml_config_file::put_client_router(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto router = entity->get<router_component>();

		boost::property_tree::iptree pt_router;
		pt_router.put(xml::address, router->get_address());
		pt_router.put(xml::port, router->get_port());

		// Option
		const auto option = entity->get<client_option_component>();
		pt_router.put(xml::message, option->get_motd());

		// Security
		const auto security = entity->get<router_security_component>();
		if (security->has_passphrase())
			pt_router.put(xml::passphrase, security->get_passphrase());

		tree.put_child(xml::router, pt_router);
	}

	void xml_config_file::put_client_security(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		// Security
		boost::property_tree::iptree pt_security;

		// Handshake
		const auto security = entity->get<client_security_component>();
		const auto message_key_size = security->get_message_key_size();
		pt_security.put(xml::message_key_size, message_key_size);

		const auto message_prime_size = security->get_message_prime_size();
		pt_security.put(xml::message_prime_size, message_prime_size);

		const auto transfer_key_size = security->get_transfer_key_size();
		pt_security.put(xml::transfer_key_size, transfer_key_size);

		const auto transfer_prime_size = security->get_transfer_prime_size();
		pt_security.put(xml::transfer_prime_size, transfer_prime_size);

		// File
		const auto hash_type = security->get_hash_type();
		pt_security.put(xml::hash_type, hash_type);

		const auto& salt = security->get_salt();
		pt_security.put(xml::salt, salt);

		tree.put_child(xml::security, pt_security);
	}

	void xml_config_file::put_router(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto router = entity->get<router_component>();

		tree.put(xml::name, router->get_name());
		tree.put(xml::address, router->get_address());
		tree.put(xml::port, router->get_port());

		// Option
		const auto option = entity->get<router_option_component>();
		if (option)
		{
			tree.put(xml::message, option->get_message());
			tree.put(xml::redirect, option->get_redirect());
			tree.put(xml::thread_pool, option->get_thread_pool());
		}			

		// Security
		const auto security = entity->get<router_security_component>();
		if (security->has_passphrase())
			tree.put(xml::passphrase, security->get_passphrase());
	}

	void xml_config_file::put_router_option(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto option = entity->get<router_option_component>();

		// Attribute
		auto& pt_attribute = tree.get_child(xml::attribute);
		pt_attribute.put(xml::enabled, option->enabled());

		// Max
		boost::property_tree::iptree pt_max;
		pt_max.put(xml::threads, option->get_max_threads());
		pt_max.put(xml::transfers, option->get_max_transfers());
		pt_max.put(xml::errors, option->get_max_errors());
		tree.put_child(xml::max, pt_max);

		// Timeout
		boost::property_tree::iptree pt_timeout;
		pt_timeout.put(xml::socket, option->get_socket_timeout());
		pt_timeout.put(xml::client, option->get_client_timeout());
		pt_timeout.put(xml::queue, option->get_queue_timeout());
		pt_timeout.put(xml::spam, option->get_spam_timeout());
		pt_timeout.put(xml::transfer, option->get_transfer_timeout());
		tree.put_child(xml::timeout, pt_timeout);

		// Transfer
		boost::property_tree::iptree pt_transfer;
		pt_transfer.put(xml::size, option->get_chunk_size());
		pt_transfer.put(xml::buffer, option->get_chunk_buffer());
		pt_transfer.put(xml::drift, option->get_chunk_drift());
		tree.put_child(xml::transfer, pt_transfer);

		// Action
		boost::property_tree::iptree pt_action;
		pt_action.put(xml::message, option->get_message_router());
		pt_action.put(xml::public_group, option->get_public_router());
		pt_action.put(xml::transfer, option->get_transfer_router());

		tree.put_child(xml::action, pt_action);
	}

	void xml_config_file::put_router_servers(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto server_vector = entity->get<server_entity_vector_component>();
		if (server_vector->empty())
			return;

		// Servers
		boost::property_tree::iptree pt_servers;

		for (const auto& entity : *server_vector)
		{
			boost::property_tree::iptree pt_server;

			// Attribute
			boost::property_tree::iptree pt_attribute;
			pt_server.add_child(xml::attribute, pt_attribute);

			// Server
			put_server(pt_server, entity);
			put_server_option(pt_server, entity);
			pt_servers.add_child(xml::server, pt_server);
		}

		tree.put_child(xml::servers, pt_servers);
	}

	void xml_config_file::put_server(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto server = entity->get<server_component>();
		tree.put(xml::name, server->get_name());
		tree.put(xml::address, server->get_address());
		tree.put(xml::port, server->get_port());

		// Security
		const auto security = entity->get<server_security_component>();
		if (security->has_passphrase())
			tree.put(xml::passphrase, security->get_passphrase());
	}

	void xml_config_file::put_server_option(boost::property_tree::iptree& tree, const entity::ptr& entity)
	{
		const auto option = entity->get<server_option_component>();

		// Attribute
		auto& pt_attribute = tree.get_child(xml::attribute);
		pt_attribute.put(xml::enabled, option->enabled());
	}

	// Accessor
	void xml_config_file::get_client(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto client = entity->get<client_component>();

		const std::string name = tree.get<std::string>(xml::name, empty_string());
		client->set_name(name);
	}

	void xml_config_file::get_client_file(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto share = entity->get<share_idle_component>();

		const auto& pt_files = tree.get_child(xml::files, empty_ptree());

		for (const auto& pair : pt_files)
		{
			const auto pt_file = pair.second;
			const auto file = pt_file.get_value<std::string>();
			share->add(file);
		}
	}

	void xml_config_file::get_client_folder(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto share = entity->get<share_idle_component>();

		const auto& pt_folders = tree.get_child(xml::folders, empty_ptree());

		for (const auto& pair : pt_folders)
		{
			const auto pt_folder = pair.second;
			const auto folder = pt_folder.get_value<std::string>();
			share->add(folder);
		}
	}

	void xml_config_file::get_client_group(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto group_vector = entity->get<group_entity_vector_component>();

		const auto& pt_groups = tree.get_child(xml::groups, empty_ptree());

		for (const auto& pair : pt_groups)
		{
			const auto pt_group = pair.second;
			const auto& pt_attribute = pt_group.get_child(xml::attribute, empty_ptree());

			// Entity
			const auto e = group_util::create(entity);

			// Attribute			
			const auto option = e->get<group_option_component>();
			const bool enabled = pt_attribute.get<bool>(xml::enabled, default_value::enabled);
			option->set_enabled(enabled);

			// Security	
			const auto file = e->get<file_component>();
			const auto path = pt_group.get<std::string>(xml::path, default_security::path);
			file->set_path(path);

			const auto security = e->get<group_security_component>();
			security->set_modified(enabled);

#if ANDROID
			char stringBuffer[6]; // 2 char for integer part, 1 char for decimal point, 2 char for fractional part, 1 char for null terminator

			snprintf(stringBuffer, 6, "%d",default_security::entropy ); // Fixes error with android mingw-gcc 4.9.3 and below, possibly higher.

			const auto str = pt_group.get<std::string>(xml::entropy, stringBuffer);
#else
			const auto str = pt_group.get<std::string>(xml::entropy, std::to_string(default_security::entropy));
#endif
			const auto entropy = boost::lexical_cast<double>(str);
			security->set_entropy(entropy);

			const auto cipher_type = pt_group.get<size_t>(xml::cipher_type, default_security::cipher_type);
			security->set_cipher_type(cipher_type);

			const auto key_size = pt_group.get<size_t>(xml::key_size, default_security::key_size);
			security->set_key_size(key_size);

			const auto hash_type = pt_group.get<size_t>(xml::hash_type, default_group::hash_type);
			security->set_hash_type(hash_type);

			const auto salt = pt_group.get<std::string>(xml::salt, default_security::salt);
			security->set_salt(salt);

			const auto iterations = pt_group.get<size_t>(xml::iterations, default_security::iterations);
			security->set_iterations(iterations);

			group_vector->add(e);
		}
	}

	void xml_config_file::get_client_option(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto option = entity->get<client_option_component>();

		// Attribute
		const auto& pt_attribute = tree.get_child(xml::attribute, empty_ptree());
		const bool enabled = pt_attribute.get<bool>(xml::enabled, default_value::enabled);
		option->set_enabled(enabled);

		// Timeout
		const auto& pt_timeout = tree.get_child(xml::timeout, empty_ptree());
		const auto socket_timeout = pt_timeout.get<size_t>(xml::socket, default_socket::num_timeout);
		option->set_socket_timeout(static_cast<size_t>(std::clamp(socket_timeout, default_socket::min_timeout, default_socket::max_timeout)));

		const auto download_timeout = pt_timeout.get<size_t>(xml::download, default_timeout::client::num_download);
		option->set_download_timeout(static_cast<size_t>(std::clamp(download_timeout, default_timeout::client::min_download, default_timeout::client::max_download)));

		const auto upload_timeout = pt_timeout.get<size_t>(xml::upload, default_timeout::client::num_upload);
		option->set_upload_timeout(static_cast<size_t>(std::clamp(upload_timeout, default_timeout::client::min_upload, default_timeout::client::max_upload)));

		const auto ping_timeout = pt_timeout.get<size_t>(xml::ping, default_timeout::client::num_ping);
		option->set_ping_timeout(static_cast<size_t>(std::clamp(ping_timeout, default_timeout::client::min_ping, default_timeout::client::max_ping)));

		// Max
		const auto& pt_max = tree.get_child(xml::max, empty_ptree());
		const auto num_errors = pt_max.get<size_t>(xml::errors, default_error::num_errors);
		option->set_max_errors(static_cast<size_t>(std::clamp(num_errors, default_error::min_errors, default_error::max_errors)));

		// Transfer
		const auto& pt_transfer = tree.get_child(xml::transfer, empty_ptree());
		const auto download_path = pt_transfer.get<std::string>(xml::path, empty_string());
		option->set_download_path(download_path);

		const auto partial = pt_transfer.get<bool>(xml::partial, default_client::partial);
		option->set_partial(partial);

		const auto num_searches = pt_transfer.get<size_t>(xml::searches, default_client::num_searches);
		option->set_max_searches(static_cast<size_t>(std::clamp(num_searches, default_client::min_searches, default_client::max_searches)));

		const auto num_downloads = pt_transfer.get<size_t>(xml::downloads, default_client::num_downloads);
		option->set_max_downloads(static_cast<size_t>(std::clamp(num_downloads, default_client::min_downloads, default_client::max_downloads)));

		const auto num_uploads = pt_transfer.get<size_t>(xml::uploads, default_client::num_uploads);
		option->set_max_uploads(static_cast<size_t>(std::clamp(num_uploads, default_client::min_uploads, default_client::max_uploads)));

		const auto num_retries = pt_transfer.get<size_t>(xml::retries, default_client::num_retries);
		option->set_max_retries(static_cast<size_t>(std::clamp(num_retries, default_client::min_retries, default_client::max_retries)));

		// Priority
		const auto& pt_priority = tree.get_child(xml::priority, empty_ptree());
		const auto priority_size = pt_priority.get<size_t>(xml::size, b_to_mb(default_client::num_priority_size));
		option->set_priority_size(static_cast<size_t>(std::clamp(mb_to_b(priority_size), default_client::min_priority_size, default_client::max_priority_size)));

		const auto priority_ext = pt_priority.get<std::string>(xml::extension, empty_string());
		option->set_priority_ext(priority_ext);

		// Threads
		const auto& pt_thread = tree.get_child(xml::thread, empty_ptree());
		const auto num_download_threads = pt_thread.get<size_t>(xml::downloads, default_client::num_download_threads);
		option->set_download_threads(static_cast<size_t>(std::clamp(num_download_threads, default_client::min_download_threads, default_client::max_download_threads)));

		const auto num_upload_threads = pt_thread.get<size_t>(xml::uploads, default_client::num_upload_threads);
		option->set_upload_threads(static_cast<size_t>(std::clamp(num_upload_threads, default_client::min_upload_threads, default_client::max_upload_threads)));

		const auto thread_size = pt_thread.get<size_t>(xml::size, b_to_mb(default_client::num_thread_size));
		option->set_thread_size(static_cast<size_t>(std::clamp(mb_to_b(thread_size), default_client::min_thread_size, default_client::max_thread_size)));

		// Chat
		const auto& pt_chat = tree.get_child(xml::chat, empty_ptree());
		const bool chat_audio = pt_chat.get<bool>(xml::audio, default_chat::audio);
		option->set_chat_audio(chat_audio);

		const bool chat_visual = pt_chat.get<bool>(xml::visual, default_chat::visual);
		option->set_chat_visual(chat_visual);

		const auto chat_path = pt_chat.get<std::string>(xml::sound, default_chat::sound);
		option->set_chat_sound(chat_path);

		const auto chat_timestamp = pt_chat.get<std::string>(xml::timestamp, default_chat::timestamp);
		option->set_chat_timestamp(chat_timestamp);

		const auto chat_num_volume = pt_chat.get<size_t>(xml::volume, default_chat::num_volume);
		option->set_chat_volume(static_cast<size_t>(std::clamp(chat_num_volume, default_chat::min_volume, default_chat::max_volume)));

		const auto chat_num_history = pt_chat.get<size_t>(xml::history, default_chat::num_history);
		option->set_chat_history(static_cast<size_t>(std::clamp(chat_num_history, default_chat::min_history, default_chat::max_history)));

		// Message
		const auto& pt_message = tree.get_child(xml::message, empty_ptree());
		const bool message_audio = pt_message.get<bool>(xml::audio, default_message::audio);
		option->set_message_audio(message_audio);

		const bool message_visual = pt_message.get<bool>(xml::visual, default_message::visual);
		option->set_message_visual(message_visual);

		const auto message_path = pt_message.get<std::string>(xml::sound, default_message::sound);
		option->set_message_sound(message_path);

		const auto message_timestamp = pt_message.get<std::string>(xml::timestamp, default_message::timestamp);
		option->set_message_timestamp(message_timestamp);

		const auto message_num_volume = pt_message.get<size_t>(xml::volume, default_message::num_volume);
		option->set_message_volume(static_cast<size_t>(std::clamp(message_num_volume, default_message::min_volume, default_message::max_volume)));

		// Action
		const auto& pt_feature = tree.get_child(xml::action, empty_ptree());
		auto value = pt_feature.get<bool>(xml::browse, default_client::action::browse);
		option->set_browse(value);

		value = pt_feature.get<bool>(xml::search, default_client::action::search);
		option->set_search(value);

		value = pt_feature.get<bool>(xml::transfer, default_client::action::transfer);
		option->set_transfer(value);

		value = pt_feature.get<bool>(xml::chat, default_client::action::chat);
		option->set_chat(value);

		value = pt_feature.get<bool>(xml::message, default_client::action::message);
		option->set_message(value);
	}

	void xml_config_file::get_client_router(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto router = entity->get<router_component>();

		const auto& pt_router = tree.get_child(xml::router, empty_ptree());
		const auto address = pt_router.get<std::string>(xml::address, empty_string());
		router->set_address(address);

		const auto port = pt_router.get<u16>(xml::port, default_network::port);
		router->set_port(port);

		// Option
		const auto motd = pt_router.get<bool>(xml::message, default_client::action::motd);
		const auto option = entity->get<client_option_component>();		
		option->set_motd(motd);

		// Security
		const auto passphrase = pt_router.get<std::string>(xml::passphrase, empty_string());
		if (!passphrase.empty())
		{
			const auto security = entity->get<router_security_component>();
			security->set_passphrase(passphrase);
		}
	}

	void xml_config_file::get_client_security(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		// Security
		const auto& pt_security = tree.get_child(xml::security, empty_ptree());

		// Handshake
		const auto security = entity->get<client_security_component>();
		const auto message_key_size = pt_security.get<size_t>(xml::message_key_size, default_security::message_key_size);
		security->set_message_key_size(message_key_size);

		const auto message_prime_size = pt_security.get<size_t>(xml::message_prime_size, default_security::message_prime_size);
		security->set_message_prime_size(message_prime_size);

		const auto transfer_key_size = pt_security.get<size_t>(xml::transfer_key_size, default_security::transfer_key_size);
		security->set_transfer_key_size(transfer_key_size);

		const auto transfer_prime_size = pt_security.get<size_t>(xml::transfer_prime_size, default_security::transfer_prime_size);
		security->set_transfer_prime_size(transfer_prime_size);

		// File
		const auto hash_type = pt_security.get<size_t>(xml::hash_type, default_security::hash_type);
		security->set_hash_type(hash_type);

		const auto salt = pt_security.get<std::string>(xml::salt, default_security::salt);
		security->set_salt(salt);
	}

	void xml_config_file::get_router(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto router = entity->get<router_component>();

		// Router
		const std::string name = tree.get<std::string>(xml::name, empty_string());
		router->set_name(name);

		const std::string address = tree.get<std::string>(xml::address, empty_string());
		router->set_address(address);

		const u16 port = tree.get<u16>(xml::port, default_network::port);
		router->set_port(port);

		// Option
		const auto option = entity->get<router_option_component>();
		if (option)
		{
			const std::string message = tree.get<std::string>(xml::message, empty_string());
			option->set_message(message);

			const std::string redirect = tree.get<std::string>(xml::redirect, empty_string());
			option->set_redirect(redirect);

			const size_t thread_pool = tree.get<size_t>(xml::thread_pool, default_router::thread_pool);
			option->set_thread_pool(thread_pool);
		}

		// Security
		const std::string passphrase = tree.get<std::string>(xml::passphrase, empty_string());
		if (!passphrase.empty())
		{
			const auto security = entity->get<router_security_component>();
			security->set_passphrase(passphrase);
		}
	}

	void xml_config_file::get_router_option(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto option = entity->get<router_option_component>();

		// Attribute
		const auto& pt_attribute = tree.get_child(xml::attribute, empty_ptree());
		const bool enabled = pt_attribute.get<bool>(xml::enabled, default_value::enabled);
		option->set_enabled(enabled);

		// Max
		const auto& pt_max = tree.get_child(xml::max, empty_ptree());
		const auto num_threads = pt_max.get<size_t>(xml::threads, default_router::num_threads);
		option->set_max_threads(static_cast<size_t>(std::clamp(num_threads, default_router::min_threads, default_router::max_threads)));

		const auto num_transfers = pt_max.get<size_t>(xml::transfers, default_router::num_transfers);
		option->set_max_transfers(static_cast<size_t>(std::clamp(num_transfers, default_router::min_transfers, default_router::max_transfers)));

		const auto num_errors = pt_max.get<size_t>(xml::errors, default_error::num_errors);
		option->set_max_errors(static_cast<size_t>(std::clamp(num_errors, default_error::min_errors, default_error::max_errors)));

		// Timeout
		const auto& pt_timeout = tree.get_child(xml::timeout, empty_ptree());
		const auto socket_timeout = pt_timeout.get<size_t>(xml::socket, default_socket::num_timeout);
		option->set_socket_timeout(static_cast<size_t>(std::clamp(socket_timeout, default_socket::min_timeout, default_socket::max_timeout)));

		const auto client_timeout = pt_timeout.get<size_t>(xml::client, default_timeout::router::num_client);
		option->set_client_timeout(static_cast<size_t>(std::clamp(client_timeout, default_timeout::router::min_client, default_timeout::router::max_client)));

		const auto queue_timeout = pt_timeout.get<size_t>(xml::queue, default_timeout::router::num_queue);
		option->set_queue_timeout(static_cast<size_t>(std::clamp(queue_timeout, default_timeout::router::min_queue, default_timeout::router::max_queue)));

		const auto spam_timeout = pt_timeout.get<size_t>(xml::spam, default_timeout::router::num_spam);
		option->set_spam_timeout(static_cast<size_t>(std::clamp(spam_timeout, default_timeout::router::min_spam, default_timeout::router::max_spam)));

		const auto transfer_timeout = pt_timeout.get<size_t>(xml::transfer, default_timeout::router::num_transfer);
		option->set_transfer_timeout(static_cast<size_t>(std::clamp(transfer_timeout, default_timeout::router::min_transfer, default_timeout::router::max_transfer)));

		// Transfer
		const auto& pt_transfer = tree.get_child(xml::transfer, empty_ptree());
		const auto chunk_size = pt_transfer.get<size_t>(xml::size, default_chunk::num_size);
		option->set_chunk_size(static_cast<size_t>(std::clamp(chunk_size, default_chunk::min_size, default_chunk::max_size)));

		const auto chunk_buffer = pt_transfer.get<size_t>(xml::buffer, default_chunk::num_buffer);
		option->set_chunk_buffer(static_cast<size_t>(std::clamp(chunk_buffer, default_chunk::min_buffer, default_chunk::max_buffer)));

		const auto chunk_drift = pt_transfer.get<size_t>(xml::drift, default_chunk::num_drift);
		option->set_chunk_drift(static_cast<size_t>(std::clamp(chunk_drift, default_chunk::min_drift, default_chunk::max_drift)));

		// Action
		const auto& pt_action = tree.get_child(xml::action, empty_ptree());

		auto value = pt_action.get<bool>(xml::message, default_router::action::message);
		option->set_message_router(value);

		value = pt_action.get<bool>(xml::public_group, default_router::action::public_group);
		option->set_public_router(value);

		value = pt_action.get<bool>(xml::transfer, default_router::action::transfer);
		option->set_transfer_router(value);
	}

	void xml_config_file::get_router_servers(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto server_vector = entity->get<server_entity_vector_component>();

		// Servers
		const auto pt_servers = tree.get_child(xml::servers, empty_ptree());

		for (const auto& pair : pt_servers)
		{
			const auto pt_server = pair.second;
			if (pt_server.empty())
				continue;

			// Entity
			const auto e = server_util::create(entity);
			get_server(pt_server, e);
			get_server_option(pt_server, e);
			server_vector->add(e);
		}
	}

	void xml_config_file::get_server(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto server = entity->get<server_component>();
		const std::string name = tree.get<std::string>(xml::name, empty_string());
		server->set_name(name);

		const std::string address = tree.get<std::string>(xml::address, empty_string());
		server->set_address(address);

		const u16 port = tree.get<u16>(xml::port, default_network::port);
		server->set_port(port);

		// Security
		const std::string passphrase = tree.get<std::string>(xml::passphrase, empty_string());
		if (!passphrase.empty())
		{
			const auto security = entity->get<server_security_component>();
			security->set_passphrase(passphrase);
		}
	}

	void xml_config_file::get_server_option(const boost::property_tree::iptree& tree, const entity::ptr& entity) const
	{
		const auto option = entity->get<server_option_component>();

		// Attribute
		const auto& pt_attribute = tree.get_child(xml::attribute, empty_ptree());
		const bool enabled = pt_attribute.get<bool>(xml::enabled, default_value::enabled);
		option->set_enabled(enabled);
	}
}
