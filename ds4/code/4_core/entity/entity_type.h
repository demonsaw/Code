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

#ifndef _EJA_ENTITY_TYPE_H_
#define _EJA_ENTITY_TYPE_H_

#include <memory>

#include "entity/entity.h"
#include "object/object.h"

namespace eja
{
	class entity_type : public object
	{
		make_factory(entity_type);

	protected:
		std::shared_ptr<entity> m_entity;

	public:
		entity_type() { }
		entity_type(const std::shared_ptr<entity>& entity) : m_entity(entity) { }

		// Operator
		operator std::shared_ptr<entity>() const { return get_entity(); }

		// Interface
		virtual void clear() override { m_entity.reset(); }

		// Utility
		virtual bool valid() const override { return m_entity != nullptr; }

		// Has
		bool has_entity() const { return m_entity != nullptr; }

		// Set
		virtual void set_entity(const std::shared_ptr<entity>& entity) { m_entity = entity; }

		// Get
		std::shared_ptr<entity> get_entity() const { return m_entity; }
	};
}

#endif
