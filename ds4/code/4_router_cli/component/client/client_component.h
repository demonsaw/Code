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

#ifndef _EJA_CLIENT_COMPONENT_H_
#define _EJA_CLIENT_COMPONENT_H_

#include <deque>
#include <string>

#include "component/endpoint_component.h"
#include "object/client.h"
#include "system/type.h"

namespace eja
{
	make_thread_safe_component(client_list_component, std::deque<std::shared_ptr<entity>>);

	class client_component final : public endpoint_component
	{
		make_factory(client_component);

		enum bits : size_t { none, verified = bit(1), share = bit(2), troll = bit(3) };

	private:
		std::string m_display;
		std::string m_seed;

	public:
		client_component() { set_seed(); }

		// Has
		bool has_display() const { return !m_display.empty(); }
		bool has_seed() const { return !m_seed.empty(); }

		// Is
		bool is_share() const { return is_mode(bits::share); }
		bool is_troll() const { return is_mode(bits::troll); }
		bool is_verified() const { return is_mode(bits::verified); }

		// Set
		void set_display() { m_display.clear(); }
		void set_display(std::string&& display) { m_display = std::move(display); }
		void set_display(const std::string& display) { m_display = display; }

		void set_seed();
		void set_seed(std::string&& seed) { m_seed = std::move(seed); }
		void set_seed(const std::string& seed) { m_seed = seed; }

		void set_share(const bool value) { set_mode(bits::share, value); }
		void set_troll(const bool value) { set_mode(bits::troll, value); }
		void set_verified(const bool value) { set_mode(bits::verified, value); }

		// Get
		const std::string& get_display() const { return m_display; }
		const std::string& get_seed() const { return m_seed; }
	};
}

#endif
