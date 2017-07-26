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

#ifndef _EJA_MODEL_COMPONENT_H_
#define _EJA_MODEL_COMPONENT_H_

#include <map>
#include <memory>

#include <QObject>

#include "component/component.h"
#include "component/callback/callback_action.h"
#include "component/callback/callback_type.h"
#include "entity/entity_callback.h"
#include "object/function.h"
#include "system/type.h"

namespace eja
{
	class model_component : public QObject, public component
	{
		Q_OBJECT;

	protected:
		callback_type m_type = callback_type::none;
		std::shared_ptr<function> m_callback;
		function_map m_functions;

	protected:
		model_component(QObject* parent = nullptr); 
		model_component(const model_component& comp) : component(comp), m_type(comp.m_type), m_callback(comp.m_callback), m_functions(comp.m_functions) { }

		// Operator
		model_component& operator=(const model_component& comp);

		// Utility
		template <typename T>
		void add_callback(const callback_action action, T t);
		void remove_callback(const callback_action action);

		template <typename ... T>
		void on_callback(const callback_action action, T ... args) const;

	protected slots:
		virtual void on_update() { on_update(nullptr); }
		virtual void on_update(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_add() { on_add(nullptr); }
		virtual void on_add(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_remove() { on_remove(nullptr); }
		virtual void on_remove(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_clear() { on_clear(nullptr); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

	signals:
		void update();
		void update(const std::shared_ptr<entity> entity);

		void add();
		void add(const std::shared_ptr<entity> entity);

		void remove();
		void remove(const std::shared_ptr<entity> entity);

		void clear();
		void clear(const std::shared_ptr<entity> entity);

	public:
		// Utility
		void reset();

		// Has
		bool has_type() const { return m_type != callback_type::none; }

		// Set
		void set_type(const callback_type type);

		// Get
		callback_type get_type() const { return m_type; }
	};

	// Callback
	template <typename T>
	void model_component::add_callback(const callback_action action, T t)
	{
		const auto function = function::create(t);
		const auto pair = std::make_pair(action, function);

		thread_lock(m_functions);
		m_functions.insert(pair);
	}

	template <typename ... T>
	void model_component::on_callback(const callback_action action, T ... args) const
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
