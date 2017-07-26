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

#ifndef _EJA_COMPONENT_H_
#define _EJA_COMPONENT_H_

#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>

#include "object/object.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_status;

	enum class callback_action;
	enum class callback_type;

	// Component
	class component : public object
	{
	private:
		std::weak_ptr<entity> m_entity;

	public:
		using ptr = std::shared_ptr<component>;

	protected:
		component() { }
		component(component&& comp) : object(comp), m_entity(std::move(comp.m_entity)) { }
		component(const component& comp) : object(comp) { }	// NOTE: Do not copy the entity weak pointer

		// Operator
		component& operator=(component&& comp);
		component& operator=(const component& comp);

	public:
		// Interface
		virtual void init() { }
		virtual void shutdown() { }
		virtual void update() { }

		// Utility
		void log(const char* psz) const;
		void log(const std::string& str) const { log(str.c_str()); }
		void log(const std::exception& ex) const { log(ex.what()); }
		void log(const boost::system::error_code& error) const { log(error.message()); }
		void log(const http_status& status) const;

		// Has
		bool has_entity() const;

		// Set
		void set_entity(const std::shared_ptr<entity> entity) { m_entity = entity; }
		void set_entity();

		// Get
		std::shared_ptr<entity> get_entity() const;
	};

	// Macro
	#define make_component(T, ...) make_type_ex(T, component, __VA_ARGS__)
	#define make_thread_safe_component(T, ...) make_thread_safe_type_ex(T, component, __VA_ARGS__)
	#define make_thread_safe_component_ex(T, ...) make_thread_safe_mutex_type_ex(T, component, std::recursive_mutex, __VA_ARGS__)
	#define make_using_component(T) make_using(T, T ## _component)
}

#endif
