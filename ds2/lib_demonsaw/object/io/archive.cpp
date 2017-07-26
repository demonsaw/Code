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

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#include "archive.h"
#include "utility/default_value.h"


namespace eja
{
	// Operator
	archive& archive::operator=(const archive& archive)
	{
		if (this != &archive)
		{
			object::operator=(archive);

			m_path = archive.m_path;
			m_size = archive.m_size;
		}

		return *this;
	}

	// Interface
	void archive::clear()
	{
		object::clear();

		m_path.clear();
		m_size = 0;
	}

	// Utility
	void archive::clean()
	{
#if _WIN32
		auto path = m_path.string();
		path.erase(boost::remove_if(path, boost::is_any_of("\\/:*?\"<>|")));
		m_path = path;
#endif
	}

	bool archive::exists() const
	{
		boost::system::error_code error;
		return boost::filesystem::exists(m_path, error);
	}
}
