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

#ifndef _EJA_NAT_COMPONENT_H_
#define _EJA_NAT_COMPONENT_H_

#include <cassert>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class nat_component final : public component
	{
		make_factory(nat_component);

	private:
		std::string m_name;
		u16 m_port;

	public:
		nat_component() : m_port(0) { }
		nat_component(const nat_component& comp) : m_name(comp.m_name), m_port(comp.m_port) { }
		nat_component(const u16 port) : m_port(port) { assert(m_port > 0); }

		// Operator
		nat_component& operator=(const nat_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return m_port > 0; }		

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_port() const { return m_port > 0; }

		// Set
		void set_name(const std::string& name) { m_name = name; }
		void set_port(const u16 port) { m_port = port; }

		// Get
		const std::string& get_name() const { return m_name; }
		u16 get_port() const { return m_port; }
	};
}

#endif
