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

#include <memory>
#include <string>
#include <boost/system/error_code.hpp>

#include "object/object.h"
#include "object/mode.h"
#include "system/type.h"

namespace eja
{
	class entity;
	class http_status;
	enum class http_code;

	// Component
	class component : public object, public mode<size_t>
	{
	private:
		std::weak_ptr<entity> m_owner;

	protected:
		std::string m_id;

	public:
		using key_type = size_t;
		using ptr = std::shared_ptr<component>;

	protected:
		component() { }
		explicit component(component&& comp) : object(comp), mode<size_t>(comp), m_id(std::move(comp.m_id)), m_owner(std::move(comp.m_owner)) { }
		explicit component(const component& comp) : object(comp), mode<size_t>(comp), m_id(comp.m_id) { }	// NOTE: Do not copy the entity weak pointer

		explicit component(const size_t value) : mode<size_t>(value) { }
		explicit component(const void* id, const size_t size) : m_id(reinterpret_cast<const char*>(id), size) { }
		explicit component(const std::string& id) : m_id(id) { }
		explicit component(const char* id) : m_id(id) { }

		virtual ~component() { }

		// Operator
		component& operator=(component&& comp);
		component& operator=(const component& comp);

	public:
		// Interface
		virtual void init() { }
		virtual void shutdown() { }
		virtual void update() { }
		virtual void clear() override;

		// Utility
		void async_call(const key_type key, const std::shared_ptr<entity>& entity) const;
		void async_call(const key_type key) const;

		void call(const key_type key, const std::shared_ptr<entity>& entity) const;
		void call(const key_type key) const;

		void log(const char* psz) const;
		void log(const std::string& str) const { log(str.c_str()); }
		void log(const std::exception& ex) const { log(ex.what()); }
		void log(const boost::system::error_code& error) const { log(error.message()); }
		void log(const http_status& status) const;
		void log(const http_code code) const;

		// Has
		bool has_id() const { return !m_id.empty(); }
		bool has_owner() const;

		// Is
		bool is_owner(const std::shared_ptr<entity>& entity) const { return get_owner() == entity; }

		// Set
		void set_id(const void* id, const size_t size) { m_id.assign(reinterpret_cast<const char*>(id), size); }
		void set_id(const std::string& id) { m_id = id; }
		void set_id(const char* id) { m_id = id; }
		void set_id() { m_id.clear(); }

		void set_owner(const std::shared_ptr<entity>& owner) { m_owner = owner; }
		void set_owner();

		// Get
		std::shared_ptr<entity> get_owner() const;
		const std::string& get_id() const { return m_id; }
	};

	// Macro
	#define make_component(T) make_type(T, component)
	#define make_component_ex(T, ...) make_type_ex(T, component, __VA_ARGS__)
	#define make_thread_safe_component(T, ...) make_thread_safe_type_ex(T, component, __VA_ARGS__)
	#define make_using_component(T) make_using(T, T ## _component)
}

#endif
