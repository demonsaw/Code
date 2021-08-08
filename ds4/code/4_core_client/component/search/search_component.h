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

#ifndef _EJA_SEARCH_COMPONENT_H_
#define _EJA_SEARCH_COMPONENT_H_

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class entity;
	
	class search_component final : public component
	{
		make_factory(search_component);

	private:		
		size_t m_swarm = 1; 
		size_t m_weight = 0;

	public:
		search_component() { }

		// Interface
		virtual void clear() override;

		// Utility
		void add_swarm(const size_t swarm = 1) { m_swarm += swarm; }
		void sub_swarm(const size_t swarm = 1) { m_swarm -= swarm; }
		
		void add_weight(const size_t weight = 1) { m_weight += weight; }
		void sub_weight(const size_t weight = 1) { m_weight -= weight; }

		// Has
		bool has_swarm() const { return m_swarm > 0; }
		bool has_weight() const { return m_weight > 0; }

		// Set
		void set_swarm(const size_t swarm) { m_swarm = swarm; }
		void set_weight(const size_t weight) { m_weight = weight; }

		// Get
		size_t get_swarm() const { return m_swarm; }
		size_t get_weight() const { return m_weight; }
	};
}

#endif
