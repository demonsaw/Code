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

#ifndef _EJA_SEARCH_
#define _EJA_SEARCH_

#include <memory>

#include "object.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	class search : public object
	{
	protected:
		size_t m_swarm = 1;

	public:
		using ptr = std::shared_ptr<search>;

	public:
		search() { }
		search(const std::string& id) : object(id) { }
		search(const char* id) : object(id) { }
		virtual ~search() override { }

		// Interface
		virtual void clear() override;

		// Utility
		void add_swarm() { ++m_swarm; }
		void add_swarm(const size_t swarm) { m_swarm += swarm; }

		// Mutator
		void set_swarm(const size_t swarm) { m_swarm = swarm; }

		// Accessor
		size_t get_swarm() const { return m_swarm; }

		// Static
		static ptr create() { return std::make_shared<search>(); }
		static ptr create(const std::string& id) { return std::make_shared<search>(id); }
		static ptr create(const char* id) { return std::make_shared<search>(id); }
	};
}

#endif
