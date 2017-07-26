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

#ifndef _EJA_ID_COMPONENT_H_
#define _EJA_ID_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class id_component : public component
	{
	protected:
		std::string m_id;

	public:
		id_component() { }
		id_component(const id_component& comp) : component(comp), m_id(comp.m_id) { }
		id_component(const byte* id, const size_t size) : m_id(reinterpret_cast<const char*>(id), size) { }
		id_component(const std::string& id) : m_id(id) { }

		// Operator
		id_component& operator=(const id_component& comp);

		bool operator==(const id_component& comp) { return m_id == comp.m_id; }
		bool operator!=(const id_component& comp) { return m_id != comp.m_id; }
		bool operator<(const id_component& comp) { return m_id < comp.m_id; }
		bool operator>(const id_component& comp) { return m_id > comp.m_id; }
		bool operator<=(const id_component& comp) { return m_id <= comp.m_id; }
		bool operator>=(const id_component& comp) { return m_id >= comp.m_id; }

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return has_id(); }

		// Has
		bool has_id() const { return !m_id.empty(); }

		// Set
		void set_id(const byte* id, const size_t size) { m_id.assign(reinterpret_cast<const char*>(id), size); }
		void set_id(const std::string& id) { m_id = id; }

		// Get
		const std::string& get_id() const { return m_id; }
	};
}

#endif
