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

#ifndef _EJA_STATE_COMPONENT_H_
#define _EJA_STATE_COMPONENT_H_

#include <boost/atomic.hpp>

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	enum class state { none, idle, running, quitting };

	class state_component final : public component
	{
		make_factory(state_component);

	private:
		boost::atomic<state> m_state;

	public:
		state_component() : m_state(state::none) { }
		state_component(const state_component& comp) { m_state.store(comp.m_state); }

		// Operator
		state_component& operator=(const state_component& comp);

		// Interface
		virtual void clear() override;

		// Utility
		virtual bool valid() const override { return !none(); }

		bool none() const { return has(state::none); }
		bool idle() const { return has(state::idle); }
		bool running() const { return has(state::running); }
		bool quitting() const { return has(state::quitting); }

		// Has
		bool has(const state value) const { return m_state.load() == value; }

		// Set
		void set(const state next) { m_state = next; }
		bool set(const state next, const state value);

		// Get
		state get() const { return m_state; }
	};
}

#endif
