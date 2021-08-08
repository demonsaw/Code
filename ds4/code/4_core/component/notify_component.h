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

#ifndef _EJA_NOTIFICATION_COMPONENT_H_
#define _EJA_NOTIFICATION_COMPONENT_H_

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class notify_component : public component
	{
		make_factory(notify_component);

	private:
		bool m_chat = false;
		bool m_pm = false;

	public:
		notify_component() { }
		notify_component(const notify_component& comp) : component(comp) { }

		// Operator
		notify_component& operator=(const notify_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		bool empty() { return !m_chat && !m_pm; }

		// Has
		bool has_chat() const { return m_chat; }
		bool has_pm() const { return m_pm; }

		// Set
		void set_chat(const bool chat) { m_chat = chat; }
		void set_pm(const bool pm) { m_pm = pm; }
	};
}

#endif
