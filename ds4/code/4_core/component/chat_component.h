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

#include "component/component.h"

#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	class entity;
	make_thread_safe_component(chat_map_component, std::map<std::string, std::shared_ptr<entity>>);	

	class chat_list_component : public thread_safe_ex<component, std::deque<std::shared_ptr<entity>>>
	{
		make_factory(chat_list_component);

		enum bits : size_t { none, notify = bit(1) };

	private:
		size_t m_rows = 0;

	public:
		chat_list_component() { }

		// Interface
		virtual void clear() override;

		// Utility
		void add() { add(1); }
		void add(const size_t rows) { m_rows += rows; }

		void remove() { remove(1); }
		void remove(const size_t rows) { m_rows -= rows; }

		// Has
		bool has_rows() const { return m_rows > 0; }

		// Is
		bool is_notify() const { return is_mode(bits::notify); }

		// Set
		void set_rows() { m_rows = 0; }
		void set_rows(const size_t rows) { m_rows = rows; }

		void set_notify(const bool value) { set_mode(bits::notify, value); }

		// Get
		size_t get_rows() const { return m_rows; }
	};

	class chat_component final : public component
	{
		make_factory(chat_component);

		enum bits : size_t { none, first = bit(1), last = bit(2) };

	private:
		std::string m_text;
		size_t m_size = 0;

	public:
		chat_component() : component(bits::first | bits::last) { }
		chat_component(std::string&& text) : component(bits::first | bits::last), m_text(std::move(text)) { }
		chat_component(const std::string& text) : component(bits::first | bits::last), m_text(text) { }

		// Interface
		virtual void clear() override;

		// Utility
		void add() { ++m_size; }
		void remove() { --m_size; }

		// Has
		bool has_size() const { return m_size > 0; }
		bool has_text() const { return !m_text.empty(); }

		// Is
		bool is_first() const { return is_mode(bits::first); }
		bool is_last() const { return is_mode(bits::last); }

		// Set
		void set_size() { m_size = 0; }
		void set_size(const size_t size) { m_size = size; }

		void set_text(std::string&& text) { m_text = std::move(text); }
		void set_text(const std::string& text) { m_text = text; }

		void set_first(const bool value) { set_mode(bits::first, value); }
		void set_last(const bool value) { set_mode(bits::last, value); }

		// Get
		size_t get_size() const { return m_size; }

		const std::string& get_text() const { return m_text; }
		std::string& get_text() { return m_text; }

		// Static
		static ptr create(std::string&& text) { return std::make_shared<chat_component>(std::move(text)); }
		static ptr create(const std::string& text) { return std::make_shared<chat_component>(text); }
	};
}

#endif
