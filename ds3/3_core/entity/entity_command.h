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

#ifndef _EJA_ENTITY_COMMAND_H_
#define _EJA_ENTITY_COMMAND_H_

#include <memory>

#include "component/group/group_component.h"
#include "data/data_packer.h"
#include "data/data_unpacker.h"
#include "entity/entity_type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class data_unpacker;
	class entity;

	enum class callback_action;
	enum class callback_type;

	class entity_command : public entity_type
	{
	protected:
		template <typename T>
		void set_list(const std::shared_ptr<T>& t) { set_list(m_entity, t); }

		template <typename T>
		void set_list(const std::shared_ptr<entity> entity, const std::shared_ptr<T>& t);

		template <typename T>
		void set_map(const std::shared_ptr<T>& t) { set_map(m_entity, t); }

		template <typename T>
		void set_map(const std::shared_ptr<entity> entity, const std::shared_ptr<T>& t);

	protected:
		entity_command() { }		
		entity_command(const entity_command& cmd) : entity_type(cmd) { }
		entity_command(const std::shared_ptr<entity> entity) : entity_type(entity) { }

		// Operator
		entity_command& operator=(const entity_command& cmd);

		// Utility
		template <typename T>
		std::string pack_data(const std::shared_ptr<T> request) const;

		// Get
		template <typename T>
		data_unpacker::ptr get_data_unpacker(const std::shared_ptr<T> response) const;
	};

	// Utility
	template <typename T>
	std::string entity_command::pack_data(const std::shared_ptr<T> request) const
	{
		data_packer packer;
		packer.push_back(request);
		auto packed = packer.pack();

		// Group
		const auto group = m_entity->get<group_component>();
		if (group->valid())
			packed = group->encrypt(packed);

		return packed;
	}

	// Set
	template <typename T>
	void entity_command::set_list(const std::shared_ptr<entity> entity, const std::shared_ptr<T>& t)
	{
		const auto comp = entity->get<T>();
		{
			thread_lock(comp);
			comp->assign(t->begin(), t->end());
		}
	}

	template <typename T>
	void entity_command::set_map(const std::shared_ptr<entity> entity, const std::shared_ptr<T>& t)
	{
		const auto comp = entity->get<T>();
		{
			thread_lock(comp);
			comp->clear();
			comp->insert(t->begin(), t->end());
		}
	}

	// Get	
	template <typename T>
	data_unpacker::ptr entity_command::get_data_unpacker(const std::shared_ptr<T> response) const
	{
		// Unpack
		const auto unpacker = data_unpacker::create();
		const auto group = m_entity->get<group_component>();

		if (group->valid())
		{
			const auto packed = group->decrypt(response->get_data());
			unpacker->unpack(packed.data(), packed.size());
		}
		else
		{
			const auto& packed = response->get_data();
			unpacker->unpack(packed.data(), packed.size());
		}

		return unpacker;
	}
}

#endif
