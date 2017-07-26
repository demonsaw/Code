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

#ifndef _EJA_TIMEOUT_COMPONENT_H_
#define _EJA_TIMEOUT_COMPONENT_H_

#include "component/component.h"
#include "object/time/timeout.h"
#include "system/type.h"

namespace eja
{
	class timeout_component : public component, public timeout
	{
		make_factory(timeout_component);

	public:
		timeout_component() { }
		timeout_component(const timeout_component& comp) : component(comp), timeout(comp) { }

		// Operator
		timeout_component& operator=(const timeout_component& comp);

		// Interface
		virtual void init() override { start(); }
		virtual void update() override { restart(); }
		virtual void clear() override { stop(); }
	};
}

#endif
