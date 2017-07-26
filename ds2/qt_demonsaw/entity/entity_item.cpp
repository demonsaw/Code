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

#include <algorithm>
#include <cassert>

#include <QtAlgorithms>

#include "entity_item.h"

namespace eja
{
	// Constructor
	entity_item::entity_item(const entity::ptr entity, const entity_item* parent)
	{ 
		m_entity = entity;
		m_parent = const_cast<entity_item*>(parent);
	}

	entity_item::~entity_item()
	{
		qDeleteAll(m_children);
	}

	// Utility
	entity_item* entity_item::get_child(const size_t pos)
	{
		if (pos >= m_children.size())
			return nullptr;

		auto it = m_children.begin();
		std::advance(it, pos);
		return *it;
	}

	size_t entity_item::get_index()
	{
		if (m_parent)
		{
			auto it = m_children.cbegin();
			for (size_t i = 0; it != m_children.cend(); ++it, ++i)
			{
				if (const_cast<entity_item*>(this) == *it)
					return i;
			}
		}			

		return 0;
	}
}
