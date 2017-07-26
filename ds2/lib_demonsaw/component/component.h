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

#ifndef _EJA_COMPONENT_
#define _EJA_COMPONENT_

#include <exception>
#include <memory>
#include <string>

#include "controller/list_controller.h"
#include "system/mutex/mutex_list.h"
#include "system/mutex/mutex_map.h"
#include "system/mutex/mutex_queue.h"
#include "system/mutex/mutex_set.h"
#include "system/mutex/mutex_vector.h"
#include "system/state.h"
#include "system/type.h"
#include "system/function/function_type.h"
#include "system/function/function_action.h"
#include "utility/default_value.h"

namespace eja
{
	// Entity
	class entity;

	// Component
	class component : public state
	{
	private:
		std::weak_ptr<entity> m_entity;

	public:
		using ptr = std::shared_ptr<component>;

	protected:
		component() { }
		virtual ~component() override { shutdown(); }

	public:
		// Utility
		void call(const function_type type, const function_action action, const std::shared_ptr<eja::entity> entity = nullptr) const; 
		bool has_entity() const { return !m_entity.expired(); }

		void log(const std::string& str) const { log(str.c_str()); }
		void log(const char* psz = default_error::unknown) const;
		void log(const std::exception& e) const { log(e.what()); }

		// Mutator		
		void set_entity(const std::shared_ptr<eja::entity> entity) { m_entity = entity; }
		void set_entity() { m_entity.reset(); }

		// Accessor
		std::shared_ptr<entity> get_entity() const { return m_entity.lock(); }
	};

	// Define
	#define derived_component(derived, ...)													\
	class derived : public __VA_ARGS__, public component									\
	{																						\
	public:																					\
		using ptr = std::shared_ptr<derived>;												\
																							\
	protected:																				\
		derived() { }																		\
		derived(const ptr p) : __VA_ARGS__(*p) { }											\
																							\
	public:																					\
		virtual ~derived() override { }														\
																							\
		static ptr create() { return ptr(new derived()); }									\
		static ptr create(const ptr p) { return ptr(new derived(p)); }						\
	}
	
	#define derived_value_component(derived, type, ...)										\
	class derived : public __VA_ARGS__, public component									\
	{																						\
	public:																					\
		using ptr = std::shared_ptr<derived>;												\
																							\
	protected:																				\
		derived() { }																		\
		derived(const ptr p) : __VA_ARGS__(*p) { }											\
		derived(const type t) : __VA_ARGS__(t) { }											\
																							\
	public:																					\
		virtual ~derived() override { }														\
																							\
		static ptr create() { return ptr(new derived()); }									\
		static ptr create(const ptr p) { return ptr(new derived(p)); }						\
		static ptr create(const type t) { return ptr(new derived(t)); }						\
	}

	#define derived_id_component(derived, ...)												\
	class derived : public __VA_ARGS__, public component									\
	{																						\
	public:																					\
		using ptr = std::shared_ptr<derived>;												\
																							\
	protected:																				\
		derived() { }																		\
		derived(const ptr p) : __VA_ARGS__(*p) { }											\
		derived(const std::string& id) : __VA_ARGS__(id) { }								\
		derived(const char* id) : __VA_ARGS__(id) { }										\
																							\
	public:																					\
		virtual ~derived() override { }														\
																							\
		static ptr create() { return ptr(new derived()); }									\
		static ptr create(const ptr p) { return ptr(new derived(p)); }						\
		static ptr create(const std::string& id) { return ptr(new derived(id)); }			\
		static ptr create(const char* id) { return ptr(new derived(id)); }					\
	}

	// Controller
	derived_type(component_list_controller, list_controller<component>);

	// Container
	template <typename T>
	derived_component(list_component, mutex_list<T>);

	template <typename K, typename V>
	derived_component(map_component, mutex_map<K, V>);

	template <typename K, typename V>
	derived_component(map_list_component, mutex_map_list<K, V>);

	template <typename T>
	derived_component(queue_component, mutex_queue<T>);

	template <typename T>
	derived_component(set_component, mutex_set<T>);

	template <typename T>
	derived_component(vector_component, mutex_vector<T>);
}

#endif
