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
#include "object/function.h"
#include "system/type.h"

namespace eja
{
	class function;

	// Status
	enum class status { none, pending, info, success, warning, error, waiting, cancelled, max };

	class status_component final : public component
	{
		make_factory(status_component);

	private:
		function_list m_callbacks;
		status m_status;

	public:
		status_component() : m_status(status::none) { }
		status_component(const status_component& comp) : m_callbacks(comp.m_callbacks), m_status(comp.m_status) { }

		// Operator
		status_component& operator=(const status_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const { return !is_none(); }

		void add(const function::ptr& cb);
		void remove(const function::ptr& cb);

		// Is
		bool is_none() const { return m_status == status::none; }
		bool is_cancelled() const { return m_status == status::cancelled; }
		bool is_error() const { return m_status == status::error; }
		bool is_info() const { return m_status == status::info; }
		bool is_pending() const { return m_status == status::pending; }
		bool is_success() const { return m_status == status::success; }
		bool is_warning() const { return m_status == status::warning; }

		// Set
		void set_status(const status& s);
		void set_status(const std::string& str) { set_status(str.c_str()); }
		void set_status(const char* psz);

		// Get
		u32 get_color() const;
		const char* get_description() const; 
		const status& get_status() const { return m_status; }
	};
}

#endif
