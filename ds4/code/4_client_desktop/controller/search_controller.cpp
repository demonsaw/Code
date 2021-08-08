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

#include <cassert>

#include "controller/search_controller.h"

#include "component/callback/callback.h"
#include "component/search/search_component.h"

#include "thread/thread_info.h"
#include "widget/search_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// COnstructor
	search_controller::search_controller(const entity::ptr& entity, QObject* parent /*= nullptr*/) : entity_controller(entity, parent)
	{
		assert(thread_info::main());

		// Callback
		add_callback(callback::search | callback::create, [&](const entity::ptr entity) { create(entity); });
		add_callback(callback::search | callback::destroy, [&](const entity::ptr entity) { destroy(entity); });
		add_callback(callback::search | callback::destroy, [&]() { destroy(); });

		add_callback(callback::search | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::search | callback::update, [&]() { update(); });

		add_callback(callback::search | callback::clear, [&](const entity::ptr entity) { clear(entity); });
		add_callback(callback::search | callback::clear, [&]() { clear(); });
	}

	// Interface
	void search_controller::on_create(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_visible_widget();
		if (widget)
		{
			widget->create();
		}
		else
		{
			const auto search = entity->get<search_component>();
			const auto widget = get_widget(search->get_id());

			if (!widget)
			{
				// Create
				const auto main = main_window::get();
				const auto window = main->get_window(m_entity);

				if (likely(window))
				{
					const auto widget = new search_widget(entity);
					window->dock(widget);
					widget->raise();

					// List
					m_list.push_back(widget);
				}
			}
			else
			{
				// Raise
				widget->raise();
			}
		}
	}

	void search_controller::on_destroy(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto search = entity->get<search_component>();
		const auto widget = get_widget(search->get_id());

		if (widget)
		{
			// List
			m_list.remove(widget);

			const auto main = main_window::get();
			const auto window = main->get_window(m_entity);
			if (likely(window))
				window->undock(widget);

			if (widget->isVisible())
			{
				widget->hide();
				widget->close();
			}

			delete widget;
		}
	}

	void search_controller::on_destroy()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
		{
			if (widget->isVisible())
			{
				widget->hide();
				widget->close();
			}

			delete widget;
		}

		// List
		m_list.clear();
	}

	void search_controller::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
			widget->add(entity);
	}

	void search_controller::on_update()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
			widget->update();
	}

	void search_controller::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
			widget->clear();
	}

	void search_controller::on_clear()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
			widget->clear();
	}

	// Get
	const std::string& search_controller::get_id(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto search = entity->get<search_component>();

		return search->get_id();
	}
}
