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

#include "controller/debug_controller.h"
#include "entity/entity.h"
#include "model/debug_model.h"
#include "thread/thread_info.h"

namespace eja
{
	// Constructor
	debug_controller::debug_controller(QObject* parent /*= nullptr*/) : entity_object(parent)
	{
		m_model = new debug_model(this);
	}

	// Interface
	void debug_controller::add(const entity::ptr entity)
	{
		assert(thread_info::main() && has_entity());

		m_model->add(entity);

		emit sizeChanged();
	}

	void debug_controller::update()
	{
		assert(thread_info::main() && has_entity());

		m_model->update();

		emit sizeChanged();
	}

	void debug_controller::clear()
	{
		assert(thread_info::main() && has_entity());

		m_model->clear();

		emit sizeChanged();
	}

	void debug_controller::reset()
	{
		assert(has_entity());

		m_model->reset();

		emit sizeChanged();
	}

	// Utility
	bool debug_controller::empty() const
	{ 
		return m_model->size() <= 1; 
	}

	// Set
	void debug_controller::set_entity(const entity::ptr entity)
	{
		entity_object::set_entity(entity);

		m_model->set_entity(entity);

		emit sizeChanged();
	}

	// Get
	size_t debug_controller::get_size() const
	{
		return m_model->size(); 
	}
}
