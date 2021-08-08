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

#ifndef _EJA_SEARCH_OPTION_COMPONENT_H_
#define _EJA_SEARCH_OPTION_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class search_option_component final : public component
	{
		make_factory(search_option_component);

	private:
		std::string m_text;

	public:
		search_option_component() { }
		search_option_component(std::string&& text) : m_text(std::move(text)) { }
		search_option_component(const std::string& text) : m_text(text) { }

		// Utility
		virtual void clear() override;

		// Has
		bool has_text() const { return !m_text.empty(); }

		// Set		
		void set_text(std::string&& text) { m_text = std::move(text); }
		void set_text(const std::string& text) { m_text = text; }
		void set_text() { m_text.clear(); }

		// Get		
		const std::string& get_text() const { return m_text; }
		std::string& get_text() { return m_text; }

		// Static
		static ptr create(std::string&& text) { return std::make_shared<search_option_component>(std::move(text)); }
		static ptr create(const std::string& text) { return std::make_shared<search_option_component>(text); }
	};
}

#endif
