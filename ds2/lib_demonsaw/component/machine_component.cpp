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

#include "machine_component.h"

namespace eja
{
	// Interface
	void machine_component::shutdown()
	{
		component::shutdown();

		m_functions.clear();
	}

	// Utility
	void machine_component::clear()
	{
		machine::clear();

		m_functions.clear();
	}

	// Mutator
	void machine_component::set_status(const eja::status status)
	{
		machine::set_status(status);

		const auto owner = get_entity();

		auto_lock_ref(m_functions);
		for (const auto& function : m_functions)
			function->call(owner);
	}

	bool machine_component::set_error(const http_status& status)
	{
		set_status(eja::status::error);
		set_state(machine_state::sleep, default_timeout::error);
		log(status);

		return true;
	}

	bool machine_component::set_error(const std::exception& ex)
	{
		set_status(eja::status::error);
		set_state(machine_state::sleep, default_timeout::error);
		log(ex);

		return true;
	}

	bool machine_component::set_error(const std::string& str)
	{
		set_status(eja::status::error);
		set_state(machine_state::sleep, default_timeout::error);
		log(str);

		return true;
	}

	bool machine_component::set_error(const char* psz)
	{
		set_status(eja::status::error);
		set_state(machine_state::sleep, default_timeout::error);
		log(psz);

		return true;
	}

	bool machine_component::set_error()
	{
		set_status(eja::status::error);
		set_state(machine_state::sleep, default_timeout::error);
		log();

		return true;
	}
}