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

#ifndef _EJA_DEBUG_CONTROLLER_H_
#define _EJA_DEBUG_CONTROLLER_H_

#include <memory>

#include "entity/entity_object.h"

namespace eja
{
	class debug_model;

	class debug_controller final : public entity_object
	{
		Q_OBJECT;

		Q_PROPERTY(size_t size READ get_size NOTIFY sizeChanged);

	private:
		debug_model* m_model = nullptr;

	public:
		debug_controller(QObject* parent = nullptr);

		// Interface
		void add(const std::shared_ptr<entity> entity);
		Q_INVOKABLE void update(); 
		Q_INVOKABLE void clear();
		Q_INVOKABLE void reset();

		// Utility
		Q_INVOKABLE bool empty() const;

		// Set
		virtual void set_entity(const std::shared_ptr<entity> entity)  override;

		// Get
		const debug_model* get_model() const { return m_model; }
		debug_model* get_model() { return m_model; }

		size_t get_size() const;

	signals:
		void sizeChanged();
	};
}

#endif
