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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)	// 'localtime': This function or variable may be unsafe.
#endif

#ifndef _EJA_ERROR_COMPONENT_H_
#define _EJA_ERROR_COMPONENT_H_

#include <chrono>
#include <ctime>
#include <string>
#include <vector>

#include "component/component.h"
#include "entity/entity.h"
#include "thread/thread_safe.h"
#include "system/type.h"

namespace eja
{
	// Container
	derived_type(error_entity_vector_component, entity_vector_component);

	class error_component final : public component
	{
		make_factory(error_component);

	private:
		std::string m_name;
		std::string m_text;
		time_t m_time = 0;

	public:
		error_component() { }
		error_component(const error_component& comp) : component(comp), m_name(comp.m_name), m_text(comp.m_text), m_time(comp.m_time) { }
		error_component(const std::string& text) { m_text = text; }

		// Operator
		error_component& operator=(const error_component& comp);

		// Interface
		virtual void init() override;

		// Has
		bool has_name() const { return !m_name.empty(); }
		bool has_text() const { return !m_text.empty(); }
		bool has_time() const { return m_time != 0; }

		// Set
		void set_name(const std::string& name) { m_name = name; }
		void set_name(const char* name) { m_name = name; }
		
		void set_text(const std::string& text) { m_text = text; }
		void set_text(const char* text) { m_text = text; }

		// Get
		const std::string& get_name() const { return m_name; }
		const std::string& get_text() const { return m_text; }
		const time_t& get_time() const { return m_time; }

		// Static
		static ptr create(const std::string& text) { return ptr(new error_component(text)); }
	};
}

#endif

