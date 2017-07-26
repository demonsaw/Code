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

#include "component/service/service_component.h"
#include "system/type.h"

namespace eja
{
	class strand_component : public service_component
	{
	protected:
		io_strand_ptr m_strand;

	protected:
		strand_component() : m_strand(io_strand_ptr(new boost::asio::io_service::strand(*get_service()))) { }
		strand_component(const strand_component& comp) : service_component(comp), m_strand(comp.m_strand) { }
		strand_component(const size_t threads) : service_component(threads), m_strand(io_strand_ptr(new boost::asio::io_service::strand(*get_service()))) { }

		// Operator
		strand_component& operator=(const strand_component& comp);

	public:
		// Utility
		virtual void stop() override;

		// Dispatch
		template <typename T>
		void dispatch(const T& t) { m_strand->dispatch(t); }
		
		// Post
		template <typename T>
		void post(const T& t) { m_strand->post(t); }

		// Get
		const io_strand_ptr get_strand() const { return m_strand; }
	};
}

#endif
