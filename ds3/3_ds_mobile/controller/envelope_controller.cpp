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

#include "component/endpoint_component.h"

#include "controller/application_controller.h"
#include "controller/envelope_controller.h"
#include "controller/message_controller.h"
#include "entity/entity.h"
#include "model/envelope_model.h"
#include "thread/thread_info.h"

namespace eja
{
	// Constructor
	envelope_controller::envelope_controller(QObject* parent /*= nullptr*/) : entity_object(parent)
	{
		m_model = new envelope_model(this);
	}

	// Interface
	void envelope_controller::add(const entity::ptr entity)
	{
		assert(thread_info::main() && has_entity());

		m_model->add(entity);

		emit sizeChanged();
	}

	void envelope_controller::remove(const int row)
	{
		assert(thread_info::main() && has_entity());

		m_model->removeRow(row);

		emit sizeChanged();
	}

	void envelope_controller::update()
	{
		assert(thread_info::main() && has_entity());

		m_model->update();

		emit sizeChanged();
	}

	void envelope_controller::restart()
	{
		assert(thread_info::main() && has_entity());

		m_model->restart();

		emit sizeChanged();
	}

	void envelope_controller::clear()
	{
		assert(thread_info::main() && has_entity());

		m_model->clear();

		emit sizeChanged();
	}

	void envelope_controller::reset()
	{
		assert(has_entity());

		m_model->reset();

		emit sizeChanged();
	}

	// Utility
	bool envelope_controller::empty() const
	{
		return m_model->empty();
	}

	void envelope_controller::mute(const int row, const bool value /*= true*/)
	{
		const auto entity = m_model->get_entity(row);

		if (entity)
		{
			const auto endpoint = entity->get<endpoint_component>();
			endpoint->set_mute(value);

			// HACK: Associate m_entity and the client list version
			//
			if (m_entity->equals<endpoint_component>(entity))
			{
				const auto endpoint = m_entity->get<endpoint_component>();
				endpoint->set_mute(value);
			}

			// UI
			const auto index = m_model->get_index(row);
			m_model->update(index);
		}
	}

	void envelope_controller::message(const int row)
	{
		assert(thread_info::main());

		const auto entity = m_model->get_entity(row);

		if (entity && entity->has_parent())
		{
			const auto app = application_controller::get();
			const auto control = app->get_message_controller();
			control->set_entity(entity);
			control->set_visible(true);
		}
	}

	// Set
	void envelope_controller::set_entity(const entity::ptr entity)
	{
		entity_object::set_entity(entity);

		m_model->set_entity(entity);

		emit sizeChanged();
	}

	// Get
	size_t envelope_controller::get_size() const
	{
		return m_model->size();
	}
}
