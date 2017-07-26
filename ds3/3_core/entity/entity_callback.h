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

#ifndef _EJA_ENTITY_CALLBACK_H_
#define _EJA_ENTITY_CALLBACK_H_

#include <cassert>
#include <memory>

#include "component/callback/callback_type.h"
#include "entity/entity_type.h"
#include "object/function.h"
#include "system/type.h"
#include "thread/thread_info.h"

namespace eja
{
	class entity;
	class function;
	enum class callback_action;

	make_thread_safe_type(function_map, std::map<callback_action, std::shared_ptr<function>>);

	class entity_callback : public entity_type
	{
	protected:
		callback_type m_type = callback_type::none;
		std::shared_ptr<function> m_callback;
		function_map m_functions;		

	protected:
		entity_callback(const callback_type type = callback_type::none);
		entity_callback(const std::shared_ptr<entity> entity, const callback_type type = callback_type::none);
		virtual ~entity_callback() override;

		// Callback
		template <typename T>
		void add_callback(const callback_action action, T t);
		void remove_callback(const callback_action action);

		template <typename ... T>
		void on_callback(const callback_action action, T ... args) const;

	public:
		// Has
		bool has_type() const { return m_type != callback_type::none; }

		// Set
		virtual void set_entity() override;
		virtual void set_entity(const std::shared_ptr<entity> entity)  override;

		// Get
		callback_type get_type() const { return m_type; }
	};

	// Callback
	template <typename T>
	void entity_callback::add_callback(const callback_action action, T t)
	{
		const auto function = function::create(t);
		const auto pair = std::make_pair(action, function);

		thread_lock(m_functions);
		m_functions.insert(pair);
	}

	template <typename ... T>
	void entity_callback::on_callback(const callback_action action, T ... args) const
	{
		thread_lock(m_functions);
		auto range = m_functions.equal_range(action);
		//assert(range.first != range.second);

		for (auto it = range.first; it != range.second; ++it)
		{
			const auto& function = it->second;
			function->call(args...);
		}
	}
}

#endif
