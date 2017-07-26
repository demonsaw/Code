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

#include <sstream>
#include <boost/lexical_cast.hpp>

#include "component/version_component.h"

namespace eja
{
	// Operator
	version_component& version_component::operator=(const version_component& comp)
	{
		if (this != &comp)
		{
			component::operator=(comp);

			m_major = comp.m_major;
			m_minor = comp.m_minor;
			m_patch = comp.m_patch;
		}

		return *this;
	}

	bool version_component::operator==(const version_component& comp)
	{
		return (m_major == comp.m_major) && (m_minor == comp.m_minor) && (m_patch == comp.m_patch);

		const auto ver = version_component::create();
		const auto ver2 = version_component::create(ver);
	}

	bool version_component::operator<(const version_component& comp)
	{
		if (m_major < comp.m_major)
			return true;
		else if (m_major > comp.m_major)
			return false;

		if (m_minor < comp.m_minor)
			return true;
		else if (m_minor > comp.m_minor)
			return false;

		return m_patch < comp.m_patch;
	}

	bool version_component::operator<=(const version_component& comp)
	{
		if (m_major < comp.m_major)
			return true;
		else if (m_major > comp.m_major)
			return false;

		if (m_minor < comp.m_minor)
			return true;
		else if (m_minor > comp.m_minor)
			return false;

		return m_patch <= comp.m_patch;
	}

	bool version_component::operator>(const version_component& comp)
	{
		if (m_major > comp.m_major)
			return true;
		else if (m_major < comp.m_major)
			return false;

		if (m_minor > comp.m_minor)
			return true;
		else if (m_minor < comp.m_minor)
			return false;

		return m_patch > comp.m_patch;
	}

	bool version_component::operator>=(const version_component& comp)
	{
		if (m_major > comp.m_major)
			return true;
		else if (m_major < comp.m_major)
			return false;

		if (m_minor > comp.m_minor)
			return true;
		else if (m_minor < comp.m_minor)
			return false;

		return m_patch >= comp.m_patch;
	}

	// Interface
	void version_component::clear()
	{
		component::clear();

		m_major = 0;
		m_minor = 0;
		m_patch = 0;
	}

	// Mutator
	void version_component::set(const std::string& str)
	{
		clear();

		size_t i = 0;
		string_tokenizer tokens(str, boost::char_separator<char>("."));

		for (const std::string& token : tokens)
		{
			const size_t value = boost::lexical_cast<size_t>(token);

			switch (i++)
			{
				case number::major:
				{
					m_major = value;
					break;
				}
				case number::minor:
				{
					m_minor = value;
					break;
				}
				case number::patch:
				{
					m_patch = value;
					break;
				}
				default:
				{
					assert(false);
				}
			}
		}
	}

	// Accessor
	std::string version_component::str() const
	{
		std::stringstream stream;
		stream << m_major << '.' << m_minor << '.' << m_patch;
		return stream.str();
	}
}
