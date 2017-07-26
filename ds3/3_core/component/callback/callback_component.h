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

#ifndef _EJA_CALLBACK_COMPONENT_H_
#define _EJA_CALLBACK_COMPONENT_H_

#include <list>
#include <memory>
#include <map>

#include "component/component.h"
#include "component/callback/callback_type.h"
#include "object/function.h"
#include "system/type.h"
#include "thread/thread_safe.h"

namespace eja
{
	class entity;

	// Containers
	class function;	
	make_thread_safe_component_ex(callback_list_component, std::list<std::shared_ptr<function>>);
	make_thread_safe_component_ex(callback_map_component, std::multimap<callback_type, std::shared_ptr<function>>);

	// Class
	class callback_component final : public component
	{
		make_factory(callback_component);

	public:
		callback_component() { }
		callback_component(const callback_component& comp) : component(comp) { }

		// Operator
		callback_component& operator=(const callback_component& comp);

		// Utility
		void add(const callback_type type, const std::shared_ptr<function>& func);
		void remove(const callback_type type, const std::shared_ptr<function>& func);
	};
}

#endif
