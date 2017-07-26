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

#include "component/model_component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_component.h"
#include "entity/entity.h"

namespace eja
{
	// Constructor
	model_component::model_component(QObject* parent /*= nullptr*/) : QObject(parent)
	{
		assert(thread_info::main());

		m_callback = function::create([&](const callback_action action, const entity::ptr entity) { on_callback(action, entity); });

		// Slots
		connect(this, static_cast<void (model_component::*)()>(&model_component::update), this, static_cast<void (model_component::*)()>(&model_component::on_update));
		connect(this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::update), this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::on_update));

		connect(this, static_cast<void (model_component::*)()>(&model_component::add), this, static_cast<void (model_component::*)()>(&model_component::on_add));
		connect(this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::add), this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::on_add));

		connect(this, static_cast<void (model_component::*)()>(&model_component::remove), this, static_cast<void (model_component::*)()>(&model_component::on_remove));
		connect(this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::remove), this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::on_remove));

		connect(this, static_cast<void (model_component::*)()>(&model_component::clear), this, static_cast<void (model_component::*)()>(&model_component::on_clear));
		connect(this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::clear), this, static_cast<void (model_component::*)(const entity::ptr)>(&model_component::on_clear));
	}

	// Operator
	model_component& model_component::operator=(const model_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_type = comp.m_type;
			m_callback = comp.m_callback;
			m_functions = comp.m_functions;
		}

		return *this;
	}

	// Utility
	void model_component::remove_callback(const callback_action action)
	{
		thread_lock(m_functions);
		m_functions.erase(action);
	}

	// Set
	void model_component::reset()
	{
		if (has_type())
		{
			const auto owner = get_entity();
			const auto callback = owner->find<callback_component>();
			callback->remove(m_type, m_callback);

			m_callback.reset();

			thread_lock(m_functions);
			m_functions.clear();
		}

		m_type = callback_type::none;
	}

	void model_component::set_type(const callback_type type)
	{
		const auto owner = get_entity();		
		const auto callback = owner->find<callback_component>();

		// Remove
		if (has_type())
			callback->remove(m_type, m_callback);
		
		// Add
		callback->add(type, m_callback);

		m_type = type;
	}
}
