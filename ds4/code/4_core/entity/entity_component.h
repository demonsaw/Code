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

#ifndef _EJA_ENTITY_COMPONENT_H_
#define _EJA_ENTITY_COMPONENT_H_

#include <memory>

#include "component/component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	class entity_component final : public component
	{
		make_factory(entity_component);

	private:
		std::shared_ptr<entity> m_entity;

	public:
		entity_component() { }

		// Interface
		virtual void init() override;

		// Utility
		virtual bool valid() const override { return m_entity != nullptr; }

		// Has
		template <typename T>
		bool has() const { return m_entity->has<T>(); }
		bool has_entity() const { return m_entity != nullptr; }

		// Get
		template <typename T>
		std::shared_ptr<T> get() const { return m_entity->get<T>(); }
		std::shared_ptr<entity> get_entity() const { return m_entity; }
	};
}

#endif

