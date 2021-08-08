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

#ifndef _EJA_SERVICE_STATE_COMPONENT_H_
#define _EJA_SERVICE_STATE_COMPONENT_H_

#include <memory>

#include "component/component.h"
#include "system/type.h"

namespace eja
{	
	class service_state_component final : public component
	{
		make_factory(service_state_component);

		enum state : size_t { none };

	private:
		size_t m_state = state::none;

	private:
		// Set
		void set_mode(const size_t state) { m_state = state; }

	public:
		service_state_component() { }
		explicit service_state_component(const size_t state) : m_state(state) { }

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return has_state(); }

		// Operator
		service_state_component& operator=(const size_t state);

		// Has
		bool has_state() const { return m_state != state::none; }

		// Utility
		bool is_state(const size_t state) { return m_state == state; }
		bool is_none() const { return m_state == state::none; }

		// Set
		void set_state() { set_none(); }
		void set_state(const size_t state) { m_state = state; }
		void set_none() { m_state = state::none; }

		// Get
		size_t get_state() const { return m_state; }

		// Static
		static ptr create(const size_t state) { return std::make_shared<service_state_component>(state); }
	};
}

#endif