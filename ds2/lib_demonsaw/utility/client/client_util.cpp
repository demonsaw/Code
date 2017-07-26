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

#include "client_util.h"

#include "component/browse_component.h"
#include "component/chat_component.h"
#include "component/chat_idle_component.h"
#include "component/function_component.h"
#include "component/search_component.h"
#include "component/session_component.h"
#include "component/share_idle_component.h"

#include "component/client/client_component.h"
#include "component/client/client_idle_component.h"
#include "component/client/client_machine_component.h"
#include "component/client/client_option_component.h"
#include "component/client/client_security_component.h"

#include "component/error/error_component.h"
#include "component/group/group_component.h"

#include "component/io/file_component.h"
#include "component/io/folder_component.h"

#include "component/router/router_component.h"
#include "component/router/router_info_component.h"
#include "component/router/router_security_component.h"

#include "component/transfer/download_component.h"
#include "component/transfer/finished_component.h"
#include "component/transfer/transfer_component.h"
#include "component/transfer/transfer_idle_component.h"
#include "component/transfer/upload_component.h"

namespace eja
{
	// Static
	mutex client_util::m_mutex;

	// Utility
	std::shared_ptr<function_map_list_component> client_util::get_function_map()
	{
		const auto function_map_list = function_map_list_component::create();

		// Client
		auto function_list = function_list_component::create();
		function_map_list->add(function_type::client, function_list);

		// Search
		function_list = function_list_component::create();
		function_map_list->add(function_type::search, function_list);

		// Browse
		function_list = function_list_component::create();
		function_map_list->add(function_type::browse, function_list);

		// Share
		function_list = function_list_component::create();
		function_map_list->add(function_type::share, function_list);

		// Chat
		function_list = function_list_component::create();
		function_map_list->add(function_type::chat, function_list);

		// Message
		function_list = function_list_component::create();
		function_map_list->add(function_type::message, function_list);

		// Group
		function_list = function_list_component::create();
		function_map_list->add(function_type::group, function_list);

		// Queue
		function_list = function_list_component::create();
		function_map_list->add(function_type::transfer, function_list);

		// Download
		function_list = function_list_component::create();
		function_map_list->add(function_type::download, function_list);

		// Upload
		function_list = function_list_component::create();
		function_map_list->add(function_type::upload, function_list);

		// Finished
		function_list = function_list_component::create();
		function_map_list->add(function_type::finished, function_list);

		// Error
		function_list = function_list_component::create();
		function_map_list->add(function_type::error, function_list);

		return function_map_list;
	}

	entity::ptr client_util::create()
	{
		const auto entity = eja::entity::create();

		const auto functions = get_function_map();
		entity->add(functions);

		// Client
		const auto client = client_component::create();
		client->set_version(default_version::app);
		entity->add(client);

		entity->add<client_entity_vector_component>(); 
		entity->add<client_option_component>();
		entity->add<client_security_component>();
		entity->add<client_machine_component>();
		entity->add<client_idle_component>();		

		entity->add<session_component>();
		entity->add<group_component>();
		entity->add<group_entity_vector_component>();

		// Share
		entity->add<share_idle_component>();
		entity->add<share_entity_vector_component>();
		entity->add<file_map_component>();
		entity->add<folder_map_component>();

		// Search, Browse, Chat
		entity->add<search_component>();
		entity->add<chat_idle_component>();
		entity->add<search_entity_vector_component>();
		entity->add<browse_entity_vector_component>();
		entity->add<chat_entity_vector_component>();
		entity->add<message_entity_vector_component>();
		entity->add<mute_entity_map_component>();

		// Router
		entity->add<router_component>();
		entity->add<router_info_component>();
		entity->add<router_security_component>();

		// Transfer		
		entity->add<transfer_entity_vector_component>();	
		entity->add<download_entity_vector_component>();
		entity->add<upload_entity_vector_component>();
		entity->add<finished_entity_vector_component>(); 
		entity->add<transfer_idle_component>();

		// Error
		entity->add<error_entity_vector_component>();

		return entity;
	}

	entity::ptr client_util::create(const entity::ptr parent)
	{
		const auto entity = create();
		entity->set_entity(parent);

		return entity;
	}
}