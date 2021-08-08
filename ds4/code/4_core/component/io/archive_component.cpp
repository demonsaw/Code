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

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#include "component/io/archive_component.h"

namespace eja
{
	// Operator
	archive_component& archive_component::operator=(const archive_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_path = comp.m_path;
			m_size = comp.m_size.load();
			m_time = comp.m_time;
		}

		return *this;
	}

	archive_component& archive_component::operator=(const boost::filesystem::path& path)
	{
		m_path = path;

		return *this;
	}

	// Interface
	void archive_component::clear()
	{
		component::clear();

		m_path.clear();
		m_size = 0;
		m_time = 0;
	}

	// Utility
	bool archive_component::exists() const
	{
		boost::system::error_code error;
		return boost::filesystem::exists(m_path, error);
	}

	/*void archive_component::clean()
	{
#if _WIN32
		auto path = m_path.string();
		path.erase(boost::remove_if(path, boost::is_any_of("\\/:*?\"<>|")));
		m_path = path;
#endif
	}*/

	// Set
	void archive_component::set_time()
	{
		boost::system::error_code error;
		m_time = boost::filesystem::last_write_time(m_path, error);
	}
}
