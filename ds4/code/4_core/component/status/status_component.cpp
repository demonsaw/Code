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
	// Constructor
	status_component::status_component()
	{
		m_color = color::white;
	}

	// Set
	void status_component::set_status(const status value)
	{
		if (m_status != value)
		{
			m_status = value;
			set_color();
		}
	}

	void status_component::set_color()
	{
		switch (m_status)
		{
			case status::success:		return set_color(color::green);
			case status::pending:		return set_color(color::gray);
			case status::info:			return set_color(color::blue);
			case status::warning:		return set_color(color::yellow);
			case status::error:			return set_color(color::red);
			case status::cancelled:		return set_color(color::black);
			default:					return set_color(color::white);
		}
	}

	// Get
	const char* status_component::get_description() const
	{
		switch (m_status)
		{
			case status::success:	return "ok";
			case status::pending:	return "pending";
			case status::info:		return "info";
			case status::warning:	return "warning";
			case status::error:		return "error";
			case status::cancelled:	return "cancel";
			default:				return "none";
		}
	}
}
