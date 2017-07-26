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

#ifndef _EJA_CHAT_COMPONENT_H_
#define _EJA_CHAT_COMPONENT_H_

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "component/component.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	class entity;

	make_thread_safe_component(chat_list_component, std::deque<std::shared_ptr<entity>>);
	make_thread_safe_component(chat_map_component, std::map<std::string, std::shared_ptr<entity>>);	

	enum class chat_type : u32 { none, system };

	// Type
	class chat_component final : public component
	{
		make_factory(chat_component);

	private:
		std::string m_client;
		std::string m_text;		
		u32 m_color = 0;

		chat_type m_type = chat_type::none;

	public:
		chat_component() { }
		chat_component(const chat_component& comp) : m_text(comp.m_text), m_client(comp.m_client), m_color(comp.m_color), m_type(comp.m_type) { }

		// Operator
		chat_component& operator=(const chat_component& comp);

		// Interface
		virtual void clear() override;

		// Has
		bool has_client() const { return !m_client.empty(); }
		bool has_text() const { return !m_text.empty(); }
		bool has_type() const { return m_type != chat_type::none; }

		// Set		
		void set_client(const std::string& client) { m_client = client; }
		void set_text(const std::string& text) { m_text = text; }
		void set_color(const u32 color) { m_color = color; }
		void set_type(const chat_type type) { m_type = type; }

		// Get
		const std::string& get_client() const { return m_client; }
		std::string& get_client() { return m_client; }

		const std::string& get_text() const { return m_text; }
		std::string& get_text() { return m_text; }

		const u32& get_color() const { return m_color; }
		u32& get_color() { return m_color; }

		const chat_type& get_type() const { return m_type; }
		chat_type& get_type() { return m_type; }
	};
}

#endif
