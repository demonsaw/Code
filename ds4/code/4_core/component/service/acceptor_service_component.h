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

#ifndef _EJA_ACCEPTOR_SERVICE_COMPONENT_H_
#define _EJA_ACCEPTOR_SERVICE_COMPONENT_H_

#include <memory>

#include "component/service/socket_service_component.h"

namespace eja
{
	class http_acceptor;

	class acceptor_service_component : public socket_service_component
	{
	private:
		std::shared_ptr<http_acceptor> m_acceptor;

	protected:
		acceptor_service_component() { }
		acceptor_service_component(const acceptor_service_component& comp) : socket_service_component(comp), m_acceptor(comp.m_acceptor) { }
		acceptor_service_component(const size_t threads) : socket_service_component(threads) { }

		// Operator
		acceptor_service_component& operator=(const acceptor_service_component& comp);

		// Has
		bool has_acceptor() const { return m_acceptor != nullptr; }

	public:
		// Utility
		virtual void start() override;
		virtual void stop() override;

		virtual bool valid() const override { return has_acceptor() && socket_service_component::valid(); }

		// Get
		std::shared_ptr<http_acceptor> get_acceptor() const { return m_acceptor; }
	};
}

#endif
