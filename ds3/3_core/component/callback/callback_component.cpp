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

#include "component/callback/callback_component.h"
#include "entity/entity.h"

namespace eja
{
	// Operator
	callback_component& callback_component::operator=(const callback_component& comp)
	{
		if (this != &comp)
			component::operator=(comp);

		return *this;
	}

	// Utility
	void callback_component::add(const callback_type type, const function::ptr& func)
	{
		// Callback
		const auto owner = get_entity();
		if (!owner)
			return;

		const auto callback_map = owner->get<callback_map_component>();
		const auto pair = std::make_pair(type, func);

		thread_lock(callback_map);
		callback_map->insert(pair);
	}

	void callback_component::remove(const callback_type type, const function::ptr& func)
	{
		// Callback
		const auto owner = get_entity();
		if (!owner)
			return;

		const auto callback_map = owner->get<callback_map_component>();

		thread_lock(callback_map);
		auto range = callback_map->equal_range(type);		
		//assert(range.first != range.second);

		for (auto it = range.first; it != range.second; )
		{
			if (func == it->second)
				callback_map->erase(it++);
			else
				++it;
		}
	}
}
