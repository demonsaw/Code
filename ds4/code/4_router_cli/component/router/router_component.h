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

#ifndef _EJA_ROUTER_COMPONENT_H_
#define _EJA_ROUTER_COMPONENT_H_

#include <deque>
#include <string>

#include "component/endpoint_component.h"
#include "component/group/group_component.h"

#include "object/router.h"
#include "system/type.h"
#include "utility/value.h"

namespace eja
{
	make_thread_safe_component(router_list_component, std::deque<std::shared_ptr<entity>>);

	class router_component final : public endpoint_component
	{
		make_factory(router_component);

		enum bits : size_t { none, message = bit(1), transfer = bit(2) };

	private:
		std::string m_motd;
		std::string m_password;
		std::string m_redirect;

		size_t m_buffer_size = network::num_buffer_size;

	public:
		router_component() : endpoint_component(bits::message | bits::transfer) { }

		// Has
		bool has_buffer_size() const { return m_buffer_size > 0; }
		bool has_motd() const { return !m_motd.empty(); }
		bool has_password() const { return !m_password.empty(); }
		bool has_redirect() const { return !m_redirect.empty(); }

		// Is
		bool is_message() const { return is_mode(bits::message); }
		bool is_transfer() const { return is_mode(bits::transfer); }

		// Set
		void set_message(const bool value) { set_mode(bits::message, value); }
		void set_transfer(const bool value) { set_mode(bits::transfer, value); }

		void set_buffer_size(const size_t buffer_size) { m_buffer_size = buffer_size; }
		void set_motd(const std::string& motd) { m_motd = motd; }
		void set_password(const std::string& password) { m_password = password; }
		void set_redirect(const std::string& redirect) { m_redirect = redirect; }

		// Get
		size_t get_buffer_size() const { return m_buffer_size; }
		const std::string& get_motd() const { return m_motd; }
		const std::string& get_password() const { return m_password; }
		const std::string& get_redirect() const { return m_redirect; }
	};
}

#endif
