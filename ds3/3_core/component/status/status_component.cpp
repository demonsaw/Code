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

#include "component/status/status_component.h"
#include "entity/entity.h"
#include "utility/value.h"

namespace eja
{
	// Operator
	status_component& status_component::operator=(const status_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_status = comp.m_status;

			// Thread-safe
			const auto& callbacks = comp.m_callbacks;
			thread_lock(callbacks);
			m_callbacks = callbacks;
		}

		return *this;
	}

	// Interface
	void status_component::clear()
	{		
		m_status = status::none;

		// Thread-safe
		thread_lock(m_callbacks);
		m_callbacks.clear();
	}

	// Utility
	void status_component::add(const function::ptr& cb)
	{
		// Thread-safe
		thread_lock(m_callbacks);
		m_callbacks->push_back(cb);
	}
	
	void status_component::remove(const function::ptr& cb)
	{
		// Thread-safe
		thread_lock(m_callbacks);
		m_callbacks->remove(cb);
	}

	// Set
	void status_component::set_status(const status& s)
	{
		// Optimization
		if (m_status == s)
			return;

		m_status = s;
		const auto entity = get_entity();

		// Thread-safe
		thread_lock(m_callbacks);
		for (const auto& callback : m_callbacks)
			callback->call(entity);
	}

	void status_component::set_status(const char* psz)
	{
		if (!strcmp(psz, "success"))
			set_status(status::success);
		else if (!strcmp(psz, "waiting"))
			set_status(status::waiting);
		else if (!strcmp(psz, "pending"))
			set_status(status::pending); 
		else if (!strcmp(psz, "info"))
			set_status(status::info); 
		else if (!strcmp(psz, "warning"))
			set_status(status::warning);
		else if (!strcmp(psz, "error"))
			set_status(status::error);
		else if (!strcmp(psz, "cancelled"))
			set_status(status::cancelled);
		else
			set_status(status::none);
	}

	// Get
	u32 status_component::get_color() const
	{
		// ARGB
		switch (m_status)
		{
			case status::none:		{ return color::white; }
			case status::success:	{ return color::green; }
			case status::waiting:	{ return color::orange; }
			case status::pending:	{ return color::gray; }
			case status::info:		{ return color::blue; }
			case status::warning:	{ return color::yellow; }
			case status::error:		{ return color::red; }
			case status::cancelled: { return color::black; }
			default:
			{
				assert(false);
				return (192 << 24) + (255 << 16) + (255 << 8) + (255 << 0);
			}
		}
	}

	const char* status_component::get_description() const
	{
		switch (m_status)
		{
			case status::success:	return "success";
			case status::waiting:	return "waiting";
			case status::pending:	return "pending"; 
			case status::info:		return "info";
			case status::warning:	return "warning";
			case status::error:		return "error";
			case status::cancelled:	return "cancelled";
			default:				return "none";
		}
	}
}
