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

#include <QLocale>

#include "entity_callback.h"
#include "component/function_component.h"
#include "window/client_window.h"
#include "window/router_window.h"
#include "window/main_window.h"

namespace eja
{
	// Interface
	void entity_callback::callback(const function_action action, const entity::ptr entity)
	{
		switch (action)
		{
			case function_action::add:
			{
				add(entity);
				break;
			}
			case function_action::remove:
			{
				remove(entity);
				break;
			}
			case function_action::refresh:
			{
				refresh(entity);
				break;
			}
			case function_action::clear:
			{
				clear(entity);
				break;
			}
			default:
			{
				assert(false);
			}
		}
	}

	// Utility
	void entity_callback::add_callback(const function_type type)
	{
		// Callback
		const auto function_map_list = m_entity->get<function_map_list_component>();
		const auto function_list = function_map_list->get(type);
		const auto function = function::create([this](const function_action action, const entity::ptr entity) { callback(action, entity); });
		function_list->add(function);
	}

	// Mutator
	void entity_callback::set_modified(const bool value)
	{
		const auto window = main_window::get_instance();
		window->setWindowModified(value);
	}

	void entity_callback::set_status(const client_statusbar index, const size_t value)
	{
		// Model
		const auto mw = main_window::get_instance();
		if (!mw)
			return;

		const auto ew = mw->get_entity_window(m_entity);
		if (!ew)
			return;

		const auto statusbar = ew->get_statusbar();
		if (!statusbar)
			return;

		const auto pane = statusbar->get_pane(static_cast<size_t>(index));
		if (pane)
		{
			const auto str = QLocale::system().toString(static_cast<uint>(value));
			pane->set_text(str);
		}
	}

	void entity_callback::set_status(const router_statusbar index, const size_t value)
	{
		// Model
		const auto mw = main_window::get_instance();
		if (!mw)
			return;

		const auto ew = mw->get_entity_window(m_entity);
		if (!ew)
			return;

		const auto statusbar = ew->get_statusbar();
		if (!statusbar)
			return;

		const auto pane = statusbar->get_pane(static_cast<size_t>(index));
		if (pane)
		{
			const auto str = QLocale::system().toString(static_cast<uint>(value));
			pane->set_text(str);
		}
	}
}
