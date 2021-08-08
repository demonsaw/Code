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

#ifndef _EJA_VERSION_COMPONENT_H_
#define _EJA_VERSION_COMPONENT_H_

#include <string>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class version_component final : public component
	{
		make_factory(version_component);

	private:
		enum number : size_t { major, minor, patch, tag }; 
		
		size_t m_major = 0;
		size_t m_minor = 0;
		size_t m_patch = 0;

	public:
		version_component() { }
		version_component(const version_component& comp) : component(comp), m_major(comp.m_major), m_minor(comp.m_minor), m_patch(comp.m_patch) { }
		version_component(const size_t major, const size_t minor = 0, const size_t patch = 0) : m_major(major), m_minor(minor), m_patch(patch) { }
		version_component(const std::string& str) { set(str); }

		// Operator
		version_component& operator=(const version_component& comp);

		bool operator==(const version_component& comp);
		bool operator!=(const version_component& comp) { return !operator==(comp); }
		bool operator<(const version_component& comp);
		bool operator<=(const version_component& comp);
		bool operator>(const version_component& comp);
		bool operator>=(const version_component& comp);

		// Interface
		virtual void clear() override;

		// Utility		
		std::string str() const;

		// Set
		void set(const std::string& str);
		void set_major(const size_t major) { m_major = major; }
		void set_minor(const size_t minor) { m_minor = minor; }
		void set_patch(const size_t patch) { m_patch = patch; }

		// Get		
		size_t get_major() const { return m_major; }
		size_t get_minor() const { return m_minor; }
		size_t get_patch() const { return m_patch; }

		// Static
		static ptr create(const size_t major, const size_t minor = 0, const size_t patch = 0) { return std::make_shared<version_component>(major, minor, patch); }
		static ptr create(const std::string& str) { return std::make_shared<version_component>(str); }
	};

}

#endif
