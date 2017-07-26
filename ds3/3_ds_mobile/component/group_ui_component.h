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

#ifndef _EJA_GROUP_Q_COMPONENT_H_
#define _EJA_GROUP_Q_COMPONENT_H_

#include <QString>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class entity;

	// Type
	class group_ui_component final : public component
	{
		make_default_factory(group_ui_component);

	private:
		QString m_initial;

	public:
		group_ui_component() { }
		group_ui_component(const group_ui_component& comp) = delete;

		// Operator
		group_ui_component& operator=(const group_ui_component& comp) = delete;

		// Interface
		virtual void clear() override;

		// Has
		bool has_initial() const { return !m_initial.isEmpty(); }

		// Set
		void set_initial(const QString& initial) { m_initial = initial; }

		// Get
		const QString& get_initial() const { return m_initial; }
		QString& get_initial() { return m_initial; }
	};
}

#endif
