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

#ifndef _EJA_STATUS_COMPONENT_H_
#define _EJA_STATUS_COMPONENT_H_

#include <list>
#include <memory>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class function;

	// Status
	enum class status { none, pending, info, success, warning, error, cancelled, max };

	class status_component final : public component
	{
		make_factory(status_component);

	private:
		u32 m_color = 0;
		status m_status = status::none;

	private:
		void set_color();
		void set_color(const u32 color) { m_color = color; }

	public:
		status_component();

		// Utility
		virtual bool valid() const override { return has_status(); }

		// Has
		bool has_status() const { return !is_none(); }

		// Is
		bool is_none() const { return m_status == status::none; }
		bool is_cancelled() const { return m_status == status::cancelled; }
		bool is_error() const { return m_status == status::error; }
		bool is_info() const { return m_status == status::info; }
		bool is_pending() const { return m_status == status::pending; }
		bool is_success() const { return m_status == status::success; }
		bool is_warning() const { return m_status == status::warning; }

		// Set
		void set_status() { set_none(); }
		void set_status(const status value);

		void set_none() { set_status(status::none); }
		void set_cancelled() { set_status(status::cancelled); }
		void set_error() { set_status(status::error); }
		void set_info() { set_status(status::info); }
		void set_pending() { set_status(status::pending); }
		void set_success() { set_status(status::success); }
		void set_warning() { set_status(status::warning); }

		// Get
		u32 get_color() const { return m_color; }
		const char* get_description() const;
		status get_status() const { return m_status; }
	};
}

#endif
