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

#ifndef _EJA_TYPE_COMPONENT_H_
#define _EJA_TYPE_COMPONENT_H_

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	enum class statusbar { none = -1, client, browse, chat, queue, download, upload, message, transfer, total };

	class statusbar_component final : public component
	{
		make_factory(statusbar_component);

	private:
		statusbar m_bar = statusbar::none;

	public:
		statusbar_component() : m_bar(statusbar::none) { }
		statusbar_component(const statusbar bar) : m_bar(bar) { }
		statusbar_component(const statusbar_component& comp) : m_bar(comp.m_bar) { }

		// Operator
		statusbar_component& operator=(const statusbar_component& comp);

		// Utility
		virtual bool valid() const { return !is_none(); }

		// Interface
		virtual void clear() override;

		// Is
		bool is_none() const { return m_bar == statusbar::none; }
		bool is_client() const { return m_bar == statusbar::client; }
		bool is_browse() const { return m_bar == statusbar::browse; }
		bool is_chat() const { return m_bar == statusbar::chat; }
		bool is_download() const { return m_bar == statusbar::download; }
		bool is_queue() const { return m_bar == statusbar::queue; }
		bool is_upload() const { return m_bar == statusbar::upload; }
		bool is_message() const { return m_bar == statusbar::message; }
		bool is_transfer() const { return m_bar == statusbar::transfer; }

		// Set
		void set(const statusbar& bar) { m_bar = bar; }

		// Get
		const statusbar& get() const { return m_bar; }

		// Create
		static ptr create(const statusbar bar) { return ptr(new statusbar_component(bar)); }
	};
}

#endif
