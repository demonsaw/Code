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

#include <algorithm>
#include <cassert>

#include <QDockWidget>

#include "entity/entity.h"
#include "entity/entity_controller.h"
#include "thread/thread_info.h"

namespace eja
{
	// Has
	bool entity_controller::has_entity(const std::string& id) const
	{
		assert(thread_info::main());

		const auto widget = get_widget(id);

		return widget ? widget->has_entity() : false;
	}

	bool entity_controller::has_widget(const std::string& id) const
	{
		assert(thread_info::main());

		const auto it = std::find_if(m_list.begin(), m_list.end(), [&](const entity_dock_widget* widget)
		{
			const auto e = widget->get_entity();
			return (id == get_id(e));
		});

		return it != m_list.end();
	}

	// Is
	bool entity_controller::is_visible() const
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
		{
			if (!widget->visibleRegion().isEmpty())
				return true;
		}

		return false;
	}

	bool entity_controller::is_visible(const void* ptr) const
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
		{
			if (widget == ptr)
				return true;
		}

		return false;
	}

	bool entity_controller::is_visible(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto& id = get_id(entity);
		const auto widget = get_widget(id);

		return widget ? !widget->visibleRegion().isEmpty() : false;
	}

	// Get
	entity::ptr entity_controller::get_entity(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto& id = get_id(entity);

		return get_entity(id);
	}

	entity::ptr entity_controller::get_entity(const std::string& id) const
	{
		assert(thread_info::main());

		const auto widget = get_widget(id);

		return widget ? widget->get_entity() : nullptr;
	}

	entity_dock_widget* entity_controller::get_widget(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto& id = get_id(entity);

		return get_widget(id);
	}

	entity_dock_widget* entity_controller::get_widget(const std::string& id) const
	{
		assert(thread_info::main());

		const auto it = std::find_if(m_list.begin(), m_list.end(), [&](const entity_dock_widget* widget)
		{
			const auto e = widget->get_entity();
			return (id == get_id(e));
		});

		return (it != m_list.end()) ? *it : nullptr;
	}

	entity::ptr entity_controller::get_visible_entity() const
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
		{
			if (!widget->visibleRegion().isEmpty())
				return widget->get_entity();
		}

		return nullptr;
	}

	entity_dock_widget* entity_controller::get_visible_widget() const
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
		{
			if (!widget->visibleRegion().isEmpty())
				return widget;
		}

		return nullptr;
	}
}
