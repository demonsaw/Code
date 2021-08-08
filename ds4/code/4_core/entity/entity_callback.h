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

#ifndef _EJA_ENTITY_CALLBACK_H_
#define _EJA_ENTITY_CALLBACK_H_

#include <memory>

#include "entity/entity_type.h"
#include "object/function.h"

namespace eja
{
	class entity;

	class entity_callback : public entity_type
	{
	public:
		using key_type = size_t;

	private:
		function_map m_map;

	protected:
		entity_callback() { }
		entity_callback(const std::shared_ptr<entity>& entity) : entity_type(entity) { }

		// Add
		template <typename T>
		void add_callback(const key_type key, const T& t);
		void add_callback(const key_type key, const function::ptr& func);

		// Remove
		void remove_callback();
		void remove_callback(const key_type key);		
		void remove_callback(const key_type key, const function::ptr& func);
		void remove_callback(const function::ptr& func);

	public:
		// Interface
		virtual void clear() override;

		// Set
		virtual void set_entity(const std::shared_ptr<entity>& entity)  override;
	};

	// Utility
	template <typename T>
	void entity_callback::add_callback(const key_type key, const T& t)
	{
		const auto func = function::create(t);
		add_callback(key, func);
	}
}

#endif
