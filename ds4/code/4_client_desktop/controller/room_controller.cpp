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

#include "controller/room_controller.h"

#include "component/room_component.h"
#include "component/callback/callback.h"
#include "component/client/client_service_component.h"

#include "thread/thread_info.h"
#include "widget/room_widget.h"
#include "window/client_window.h"
#include "window/main_window.h"

namespace eja
{
	// COnstructor
	room_controller::room_controller(const entity::ptr& entity, QObject* parent /*= nullptr*/) : entity_controller(entity, parent)
	{
		assert(thread_info::main());

		// Callback
		add_callback(callback::room | callback::create, [&](const entity::ptr entity) { create(entity); });
		add_callback(callback::room | callback::destroy, [&](const entity::ptr entity) { destroy(entity); });
		add_callback(callback::room | callback::destroy, [&]() { destroy(); });

		add_callback(callback::room | callback::add, [&](const entity::ptr entity) { add(entity); });
		add_callback(callback::room | callback::update, [&](const entity::ptr entity) { update(entity); });
		add_callback(callback::room | callback::update, [&]() { update(); });

		add_callback(callback::room | callback::clear, [&](const entity::ptr entity) { clear(entity); });
		add_callback(callback::room | callback::clear, [&]() { clear(); });
	}

	// Interface
	void room_controller::on_create(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto room = entity->get<room_component>();
		const auto widget = get_widget(room->get_id());

		if (!widget)
		{
			// Room
			room->add_size();

			// Create
			const auto main = main_window::get();
			const auto window = main->get_window(m_entity);

			if (likely(window))
			{
				const auto widget = new room_widget(entity);
				window->dock(widget);
				widget->raise();

				// Scroll
				widget->auto_scroll();

				// List
				m_list.push_back(widget);

				// Service
				const auto client_service = m_entity->get<client_service_component>();
				client_service->async_room_join(entity);
			}

			// Callback
			m_entity->async_call(callback::client | callback::update, entity);
		}
		else
		{
			// Raise
			widget->raise();

			// Scroll
			static_cast<room_widget*>(widget)->auto_scroll();
		}
	}

	void room_controller::on_destroy(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto room = entity->get<room_component>();
		const auto widget = get_widget(room->get_id());

		if (widget)
		{
			// Service
			const auto client_service = m_entity->get<client_service_component>();
			client_service->async_room_quit(entity);
			room->sub_size();

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

	void room_controller::on_destroy()
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

	void room_controller::on_add(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
		{
			widget->add(entity);

			// Scroll
			static_cast<room_widget*>(widget)->auto_scroll();
		}			
	}

	void room_controller::on_update(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
			widget->update();
	}

	void room_controller::on_update()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
			widget->update();
	}

	void room_controller::on_clear(const entity::ptr entity)
	{
		assert(thread_info::main());

		const auto widget = get_widget(entity);
		if (widget)
			widget->clear();
	}

	void room_controller::on_clear()
	{
		assert(thread_info::main());

		for (const auto& widget : m_list)
			widget->clear();
	}

	// Get
	const std::string& room_controller::get_id(const entity::ptr& entity) const
	{ 
		assert(thread_info::main());

		const auto room = entity->get<room_component>();

		return room->get_id();
	}
}
