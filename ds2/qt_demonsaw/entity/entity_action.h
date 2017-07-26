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

#ifndef _EJA_ENTITY_ACTION_
#define _EJA_ENTITY_ACTION_

#include <QAction>

#include "entity/entity.h"

namespace eja
{
	class entity_action : public QAction
	{
		Q_OBJECT

	protected:
		entity::ptr m_entity = nullptr;

	public:
		entity_action(QWidget* parent = 0) : QAction(parent) { }
		entity_action(const QString& text, QWidget* parent = 0) : QAction(text, parent) { }
		entity_action(const QIcon& icon, const QString& text, QWidget* parent = 0) : QAction(icon, text, parent) { }
		virtual ~entity_action() override { }

		// Utility
		bool valid() const { return m_entity != nullptr; }
		bool invalid() const { return !valid(); }

		// Mutator
		void set_entity(const entity::ptr entity) { m_entity = entity; }

		// Accessor
		entity::ptr get_entity() const { return m_entity; }
	};
}

#endif
