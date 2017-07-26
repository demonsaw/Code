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

#ifndef _EJA_FUNCTION_COMPONENT_
#define _EJA_FUNCTION_COMPONENT_

#include "component.h"
#include "system/function/function.h"
#include "system/function/function_type.h"
#include "system/type.h"

namespace eja
{
	// Component
	class function_component final : public function, public component
	{
	public:
		using ptr = std::shared_ptr<function_component>;

	private:
		function_component() { }
		template <typename T> function_component(T value) : function(value) { }

	public:		
		virtual ~function_component() override { }

		// Static
		static ptr create() { return ptr(new function_component()); }
		template <typename T> static ptr create(T value) { return ptr(new function_component(value)); }
	};

	// Container
	derived_type(function_list_component, list_component<function>);
	derived_type(function_map_component, map_component<function_type, function>);
	derived_type(function_map_list_component, map_list_component<function_type, function>);
	derived_type(function_queue_component, queue_component<function>);
	derived_type(function_vector_component, vector_component<function>);
}

#endif
