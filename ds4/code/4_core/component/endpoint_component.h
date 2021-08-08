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

#ifndef _EJA_ENDPOINT_COMPONENT_H_
#define _EJA_ENDPOINT_COMPONENT_H_

#include <string>
#include <boost/format.hpp>

#include "component/name_component.h"
#include "system/type.h"

namespace eja
{
	class entity;

	class endpoint_component : public name_component
	{
		make_factory(endpoint_component);

	protected:
		std::string m_address;
		u16 m_port = 0;

	public:
		endpoint_component() { }
		endpoint_component(const endpoint_component& comp) : name_component(comp), m_address(comp.m_address), m_port(comp.m_port) { }
		endpoint_component(const size_t value) : name_component(value) { }

		// Operator
		endpoint_component& operator=(const endpoint_component& comp);

		// Utility
		virtual bool valid() const override { return has_endpoint(); }

		// Has
		bool has_endpoint() const { return has_address() && has_port(); }
		bool has_address() const { return !m_address.empty(); }
		bool has_port() const { return m_port > 0; }

		// Set
		void set_address() { m_address.clear(); }
		void set_address(std::string&& address) { m_address = std::move(address); }
		void set_address(const std::string& address) { m_address = address; }

		void set_port() { m_port = 0; }
		void set_port(const u16 port) { m_port = port; }

		// Get
		std::string get_endpoint() const { return boost::str(boost::format("%s:%u") % m_address % m_port); }
		const std::string& get_address() const { return m_address; }
		u16 get_port() const { return m_port; }
	};

	// Operator
	inline endpoint_component& endpoint_component::operator=(const endpoint_component& comp)
	{
		if (this != &comp)
		{
			name_component::operator=(comp);

			m_address = comp.m_address;
			m_port = comp.m_port;
		}

		return *this;
	}
}
#endif
