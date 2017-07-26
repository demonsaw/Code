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

#ifndef _EJA_GROUP_STATUS_COMPONENT_
#define _EJA_GROUP_STATUS_COMPONENT_

#include "component/status_component.h"
#include "object/status.h"

namespace eja
{
	// Component
	class group_status_component : public status_component
	{
	public:
		using ptr = std::shared_ptr<group_status_component>;

	public:
		group_status_component() { }
		group_status_component(const eja::status status) : status_component(status) { }
		virtual ~group_status_component() override { }

		// Mutator
		virtual void set_status(const eja::status status) override;

		// Static
		static ptr create() { return ptr(new group_status_component()); }
		static ptr create(const eja::status status) { return ptr(new group_status_component(status)); }
	};
}

#endif
