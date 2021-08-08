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

#ifndef _EJA_STRAND_COMPONENT_H_
#define _EJA_STRAND_COMPONENT_H_

#include "component/component.h"
#include "system/type.h"

namespace eja
{
	class strand_component final : public component
	{
		make_param_factory(strand_component);

	private:
		io_service_ptr m_service;
		io_strand_ptr m_strand;

	public:
		explicit strand_component(const io_service_ptr& service);
		
		// Dispatch
		template <typename T>
		void dispatch(const T& t) { reinterpret_cast<const strand_component*>(this)->dispatch(t); }

		template <typename T>
		void dispatch(const T& t) const;

		// Post
		template <typename T>
		void post(const T& t) { reinterpret_cast<const strand_component*>(this)->post(t); }

		template <typename T>
		void post(const T& t) const;

		// Utility
		virtual bool valid() const override { return !m_service->stopped(); }

		// Get
		io_service_ptr get_service() const { return m_service; }
		io_strand_ptr get_strand() const { return m_strand; }

		// Static
		static ptr create(const io_service_ptr& service) { return std::make_shared<strand_component>(service); }
	};

	// Dispatch
	template <typename T>
	void strand_component::dispatch(const T& t) const
	{
		if (valid())
			m_strand->dispatch([t]() { t(); });
	}

	// Post
	template <typename T>
	void strand_component::post(const T& t) const
	{
		if (valid())
			m_strand->post([t]() { t(); });
	}
}

#endif
