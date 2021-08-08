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

#ifndef _EJA_ERROR_COMPONENT_H_
#define _EJA_ERROR_COMPONENT_H_

#include <deque>
#include <string>

#include "component/component.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	class entity;

	make_thread_safe_component(error_list_component, std::deque<std::shared_ptr<entity>>);

	class error_component final : public component
	{
		make_factory(error_component);

	private:
		std::string m_text;

	public:
		error_component() { }
		error_component(const std::string& text) : m_text(text) { }

		// Has
		bool has_text() const { return !m_text.empty(); }

		// Set
		void set_text(const std::string& text) { m_text = text; }

		// Get
		const std::string& get_text() const { return m_text; }

		// Static
		static ptr create(const std::string& text) { return std::make_shared<error_component>(text); }
	};
}

#endif
