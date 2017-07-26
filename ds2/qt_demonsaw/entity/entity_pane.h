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

#ifndef _EJA_ENTITY_PANE_
#define _EJA_ENTITY_PANE_

#include <QWidget>

#include "entity/entity.h"
#include "entity/entity_action.h"
#include "entity/entity_callback.h"

namespace eja
{
	class entity_pane : public QWidget, public entity_callback
	{
		Q_OBJECT

	public:
		static const char* action;

	protected:
		entity_pane(const entity::ptr entity, QWidget* parent = 0) : QWidget(parent), entity_callback(entity) { }
		virtual ~entity_pane() { }

		// Interface
		virtual void create() = 0;
		virtual void layout() = 0;
		virtual void signal() = 0;

	public:
		// Accessor
		entity_action* get_action() const;
	};
}

#endif
