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

#ifndef _EJA_HASH_COMPONENT_H_
#define _EJA_HASH_COMPONENT_H_

#include <string>

#include "component/component.h"

namespace eja
{
	class hash_component : public component
	{
	protected:
		u32 m_hash = 0;

	public:
		hash_component() { }
		hash_component(const hash_component& comp) : component(comp), m_hash(comp.m_hash) { }
		hash_component(const void* id, const size_t size) { set_hash(id, size); }
		hash_component(const std::string& id) { set_hash(id); }
		hash_component(const char* id) { set_hash(id); }

		// Operator
		hash_component& operator=(const hash_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return has_hash(); }

		// Has
		bool has_hash() const { return m_hash > 0; }

		// Set
		void set_hash(const void* input, const size_t input_size);
		void set_hash(const std::string& input) { return set_hash(input.c_str(), input.size()); }
		void set_hash(const char* input) { return set_hash(input, strlen(input)); }

		// Get
		u32 get_hash() const { return m_hash; }

		// Static
		static ptr create(const void* id, const size_t size) { return std::make_shared<hash_component>(id, size); }
		static ptr create(const std::string& id) { return std::make_shared<hash_component>(id); }
		static ptr create(const char* id) { return std::make_shared<hash_component>(id); }
	};
}

#endif
