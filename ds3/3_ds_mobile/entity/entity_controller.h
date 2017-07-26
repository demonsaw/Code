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

#ifndef _EJA_ENTITY_CONTROLLER_H_
#define _EJA_ENTITY_CONTROLLER_H_

#include <QObject>

#include "component/callback/callback_type.h"
#include "entity/entity_callback.h"

namespace eja
{
	class entity;

	class entity_controller : public QObject, public entity_callback
	{
		Q_OBJECT;

	protected slots:
		virtual void on_create() { on_create(nullptr); }
		virtual void on_create(const std::shared_ptr<entity> entity) { assert(false); }		

		virtual void on_destroy() { on_destroy(nullptr); }
		virtual void on_destroy(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_init() { on_init(nullptr); }
		virtual void on_init(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_update() { on_update(nullptr); }
		virtual void on_update(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_shutdown() { on_shutdown(nullptr); }
		virtual void on_shutdown(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_add() { on_add(nullptr); }
		virtual void on_add(const std::shared_ptr<entity> entity) { assert(false); }
		
		virtual void on_remove() { on_remove(nullptr); }
		virtual void on_remove(const std::shared_ptr<entity> entity) { assert(false); }

		virtual void on_clear() { on_clear(nullptr); }
		virtual void on_clear(const std::shared_ptr<entity> entity) { assert(false); }

	signals:
		void create();
		void create(const std::shared_ptr<entity> entity);

		void destroy();
		void destroy(const std::shared_ptr<entity> entity);

		void init();
		void init(const std::shared_ptr<entity> entity);

		void update();
		void update(const std::shared_ptr<entity> entity);

		void shutdown();
		void shutdown(const std::shared_ptr<entity> entity);

		void add();
		void add(const std::shared_ptr<entity> entity);

		void remove();
		void remove(const std::shared_ptr<entity> entity);

		void clear();
		void clear(const std::shared_ptr<entity> entity);

	public:
		entity_controller(const callback_type type = callback_type::none, QObject* parent = nullptr);
		entity_controller(const std::shared_ptr<entity> entity, const callback_type type = callback_type::none, QObject* parent = nullptr);
	};
}

#endif
