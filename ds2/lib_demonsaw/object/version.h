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

#ifndef _EJA_VERSION_
#define _EJA_VERSION_

#include <string>
#include "object.h"

namespace eja
{
	class version : object
	{
		enum column : size_t { major, minor, patch };

	private:
		size_t m_major = 0;
		size_t m_minor = 0;
		size_t m_patch = 0;

	public:
		version() { }
		version(const std::string& str) { set(str); }
		version(const size_t major, const size_t minor = 0, const size_t patch = 0) : m_major(major), m_minor(minor), m_patch(patch) { }

		// Operator
		bool operator==(const eja::version& version);
		bool operator!=(const eja::version& version) { return !operator==(version); }

		bool operator<(const eja::version& version);
		bool operator<=(const eja::version& version);
		bool operator>(const eja::version& version);
		bool operator>=(const eja::version& version);

		// Utility
		void clear();		

		// Mutator
		void set(const std::string& str);
		void set_major(const size_t major) { m_major = major; }
		void set_minor(const size_t minor) { m_minor = minor; }
		void set_patch(const size_t patch) { m_patch = patch; }

		// Accessor
		std::string get() const;
		size_t get_major() const { return m_major; }
		size_t get_minor() const { return m_minor; }
		size_t get_patch() const { return m_patch; }
	};
}

#endif
