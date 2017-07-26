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

#ifndef _EJA_MESSAGE_NETWORK_COMPONENT_H_
#define _EJA_MESSAGE_NETWORK_COMPONENT_H_

#include <string>
#include <vector>

#include "component/component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class message_network_component : public component
	{
		make_factory(message_network_component);

	private:
		std::string m_motd;
		std::string m_redirect;
		size_t m_buffer = 0;

		// Router
		bool m_message = false;
		bool m_transfer = false;
		bool m_proxy = false;
		bool m_open = false;

	public:
		message_network_component();
		message_network_component(const message_network_component& comp) : component(comp), m_motd(comp.m_motd), m_redirect(comp.m_redirect),
			m_buffer(comp.m_buffer), m_message(comp.m_message), m_transfer(comp.m_transfer), m_proxy(comp.m_proxy), m_open(comp.m_open) { }

		// Operator
		message_network_component& operator=(const message_network_component& comp);

		// Interface
		virtual void clear() override;

		// Has
		bool has_motd() const { return !m_motd.empty(); }
		bool has_redirect() const { return !m_redirect.empty(); }
		bool has_buffer() const { return m_buffer > 0; }

		bool has_message() const { return m_message; }
		bool has_transfer() const { return m_transfer; }
		bool has_proxy() const { return m_proxy; }
		bool has_open() const { return m_open; }

		// Set
		void set_motd(const std::string& motd) { m_motd = motd; }
		void set_redirect(const std::string& redirect) { m_redirect = redirect; }
		void set_buffer(const size_t buffer) { m_buffer = buffer; }

		void set_message(const bool message) { m_message = message; }
		void set_transfer(const bool transfer) { m_transfer = transfer; }
		void set_proxy(const bool proxy) { m_proxy = proxy; }
		void set_open(const bool open) { m_open = open; }

		// Get
		const std::string& get_motd() const { return m_motd; }
		const std::string& get_redirect() const { return m_redirect; }
		size_t get_buffer() const { return m_buffer; }
	};
}

#endif
