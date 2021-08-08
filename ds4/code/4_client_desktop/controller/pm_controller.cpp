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

#include "controller/pm_controller.h"

#include "component/chat_component.h"
#include "component/callback/callback.h"
#include "component/client/client_component.h"

#include "thread/thread_info.h"
#include "widget/pm_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// COnstructor
	pm_controller::pm_controller(const entity::ptr& entity, QObject* parent /*= nullptr*/) : entity_controller(entity, parent)
	{
		assert(thread_info::main());

		// Callback
		add_callback(callback::pm | callback::create, [&](const entity::ptr entity) { create(entity); });
		add_callback(callback::pm | callback::destroy, [&](const entity::ptr entity) { destroy(entity); });
		add_callback(callback::pm | callback::destroy, [&]() { destroy(); });

		add_callback(callback::pm | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::pm | callback::update, [&](const entity::ptr entity) { update(entity); });
		add_callback(callback::pm | callback::update, [&]() { update(); });

		add_callback(callback::pm | callback::clear, [&](const entity::ptr entity) { clear(entity); });
		add_callback(callback::pm | callback::clear, [&]() { clear(); });
	}

	// Interface
	void pm_controller::on_create(const entity::ptr entity)
	{
		assert(thread_info::main());
		assert(entity != m_entity);

		const auto client = entity->get<client_component>();
		const auto widget = get_widget(client->get_id());

		if (!widget)
		{
			// Create
			const auto main = main_window::get();
			const auto window = main->get_window(m_entity);

			if (likely(window))
			{
				const auto widget = new pm_widget(entity);
				window->dock(widget);				
				widget->raise();

				// Scroll
				widget->auto_scroll();

				// List
				m_list.push_back(widget);
			}
		}
		else
		{
			// Raise
			widget->raise();

			// Scroll
			static_cast<pm_widget*>(widget)->auto_scroll();
		}
	}

	void pm_controller::on_destroy(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto client = entity->get<client_component>();
		const auto widget = get_widget(client->get_id());

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

	void pm_controller::on_destroy()
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

	void pm_controller::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto owner = entity->get_owner();
		const auto client = owner->get<client_component>();
		const auto widget = get_widget(client->get_id());

		if (widget)
		{
			widget->add(entity);

			// Scroll
			static_cast<pm_widget*>(widget)->auto_scroll();
		}
		else
		{
			// Auto Open
			const auto& app = main_window::get_app();
			if (app.is_auto_open())
			{
				// Create
				const auto main = main_window::get();
				const auto window = main->get_window(m_entity);

				if (likely(window))
				{
					const auto widget = new pm_widget(owner);
					window->dock(widget);

					// Add
					widget->add(entity);

					// Scroll
					widget->auto_scroll();

					// List
					m_list.push_back(widget);
				}
			}
			else
			{
				const auto chat_list = owner->get<chat_list_component>();
				chat_list->push_back(entity);
				chat_list->add();

				// Notify
				chat_list->set_notify(true);

				// Callback
				m_entity->async_call(callback::client | callback::update, owner);
			}
		}
	}

	void pm_controller::on_update(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
			widget->update();
	}

	void pm_controller::on_update()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
			widget->update();
	}

	void pm_controller::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
		{
			widget->clear();
		}
		else
		{
			const auto chat_list = entity->get<chat_list_component>();
			chat_list->clear();
		}

		// Callback
		m_entity->async_call(callback::client | callback::update, entity);
	}

	void pm_controller::on_clear()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
			widget->clear();

		// Callback
		m_entity->async_call(callback::client | callback::update);
	}

	// Get
	const std::string& pm_controller::get_id(const entity::ptr& entity) const
	{
		assert(thread_info::main());

		const auto client = entity->get<client_component>();

		return client->get_id();
	}
}
