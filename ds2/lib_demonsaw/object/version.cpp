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

#if ANDROID
#include <sstream>
#else
#include <strstream>
#endif

#include <boost/lexical_cast.hpp>

#include "version.h"
#include "utility/boost.h"

namespace eja
{
	// Operator
	bool version::operator==(const eja::version& version)
	{
		return (m_major == version.m_major) && (m_minor == version.m_minor) && (m_patch == version.m_patch);
	}

	bool version::operator<(const eja::version& version)
	{
		if (m_major < version.m_major)
			return true;
		else if (m_major > version.m_major)
			return false;

		if (m_minor < version.m_minor)
			return true;
		else if (m_minor > version.m_minor)
			return false;

		return m_patch < version.m_patch;
	}

	bool version::operator<=(const eja::version& version)
	{
		if (m_major < version.m_major)
			return true;
		else if (m_major > version.m_major)
			return false;

		if (m_minor < version.m_minor)
			return true;
		else if (m_minor > version.m_minor)
			return false;

		return m_patch <= version.m_patch;
	}

	bool version::operator>(const eja::version& version)
	{
		if (m_major > version.m_major)
			return true;
		else if (m_major < version.m_major)
			return false;

		if (m_minor > version.m_minor)
			return true;
		else if (m_minor < version.m_minor)
			return false;

		return m_patch > version.m_patch;
	}

	bool version::operator>=(const eja::version& version)
	{
		if (m_major > version.m_major)
			return true;
		else if (m_major < version.m_major)
			return false;

		if (m_minor > version.m_minor)
			return true;
		else if (m_minor < version.m_minor)
			return false;

		return m_patch >= version.m_patch;
	}

	// Utility
	void version::clear()
	{
		m_major = 0;
		m_minor = 0;
		m_patch = 0;
	}

	// Mutator
	void version::set(const std::string& str)
	{
		clear();

		size_t i = 0;
		boost::string_tokenizer tokens(str, boost::char_separator<char>("."));		

		for (const std::string& token : tokens)
		{
			const size_t value = boost::lexical_cast<size_t>(token);

			switch (i++)
			{
				case column::major:
				{
					m_major = value;
					break;
				}
				case column::minor:
				{
					m_minor = value;
					break;
				}
				case column::patch:
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
	std::string version::get() const
	{
		std::stringstream stream;
		stream << m_major << '.' << m_minor << '.' << m_patch;
		return stream.str();
	}
}
