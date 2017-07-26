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

#ifndef _EJA_VERSION_COMPONENT_
#define _EJA_VERSION_COMPONENT_

#include <memory>

#include "component.h"
#include "object/version.h"

namespace eja
{
	// Component
	class version_component : public version, public component
	{
	public:
		using ptr = std::shared_ptr<version_component>;

	protected:
		version_component() { }
		version_component(const std::string& str) : version(str) { }
		version_component(const size_t major, const size_t minor = 0, const size_t patch = 0) : version(major, minor, patch) { }

	public:
		virtual ~version_component() override { }

		// Static
		static ptr create() { return ptr(new version_component()); }
		static ptr create(const std::string& str) { return ptr(new version_component(str)); }
		static ptr create(const size_t major, const size_t minor = 0, const size_t patch = 0) { return ptr(new version_component(major, minor, patch)); }
	};

}

#endif
