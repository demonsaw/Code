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

#include "group_util.h"
#include "component/function_component.h"
#include "component/group/group_component.h"
#include "component/group/group_cipher_component.h"
#include "component/group/group_option_component.h"
#include "component/group/group_security_component.h"
#include "component/group/group_status_component.h"
#include "component/io/file_component.h"

namespace eja
{
	// Utility
	std::shared_ptr<function_map_list_component> group_util::get_function_map()
	{
		const auto function_map_list = function_map_list_component::create();

		// Group
		auto function_list = function_list_component::create();
		function_map_list->add(function_type::group, function_list);

		// Error
		function_list = function_list_component::create();
		function_map_list->add(function_type::error, function_list);

		return function_map_list;
	}

	entity::ptr group_util::create()
	{
		const auto entity = eja::entity::create();

		const auto functions = get_function_map();
		entity->add(functions);
		
		entity->add<group_cipher_component>(); 
		entity->add<group_option_component>();
		entity->add<group_status_component>();
		entity->add<group_security_component>();
		entity->add<file_component>();

		return entity;
	}

	entity::ptr group_util::create(const entity::ptr parent)
	{
		const auto entity = create();
		entity->set_entity(parent);

		return entity;
	}
}