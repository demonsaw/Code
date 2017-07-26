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

#ifndef _EJA_CHAT_
#define _EJA_CHAT_

#include "object.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_vector.h"
#include "system/type.h"
#include "utility/default_value.h"

namespace eja
{
	enum class chat_type { group, client, router };

	class chat : public object
	{
	protected:
		bool m_unread = false;
		chat_type m_type = chat_type::group; 
		std::string m_text;
		u32 m_color = 0;

	public:
		using ptr = std::shared_ptr<chat>;

	public:
		chat() { }
		chat(const chat& chat) : object(chat), m_type(chat.m_type), m_text(chat.m_text), m_color(chat.m_color) { }
		chat(const std::string& id) : object(id) { }
		chat(const char* id) : object(id) { }
		chat(const chat_type type) : m_type(type) { }
		virtual ~chat() override { }

		// Interface
		virtual void clear() override;

		// Utility		
		bool has_text() const { return !m_text.empty(); }
		bool has_unread() const { return m_unread; }

		// Mutator
		void set_type(const chat_type type) { m_type = type; }
		void set_text(const std::string& text) { m_text = text; }
		void set_text(const char* text) { m_text = text; }
		void set_color(const u32 color) { m_color = color; }

		void set_read() { m_unread = false; }
		void set_unread() { m_unread = true; }

		// Accessor
		chat_type get_type() const { return m_type; }
		const std::string& get_text() const { return m_text; }
		const u32 get_color() const { return m_color; }

		// Static
		static ptr create() { return std::make_shared<chat>(); }
		static ptr create(const chat& chat) { return std::make_shared<eja::chat>(chat); }
		static ptr create(const chat::ptr chat) { return std::make_shared<eja::chat>(*chat); }
		static ptr create(const std::string& id) { return std::make_shared<chat>(id); }
		static ptr create(const char* id) { return std::make_shared<chat>(id); }
		static ptr create(const chat_type type) { return std::make_shared<chat>(type); }
	};

	// Container
	derived_type(chat_list, mutex_list<chat>);
	derived_type(chat_map, mutex_map<std::string, chat>);
	derived_type(chat_queue, mutex_queue<chat>);
	derived_type(chat_vector, mutex_vector<chat>);
}

#endif
