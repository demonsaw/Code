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

#include "component/service/acceptor_service_component.h"

#include "http/http_acceptor.h"

namespace eja
{
	// Operator
	acceptor_service_component& acceptor_service_component::operator=(const acceptor_service_component& comp)
	{
		if (this != &comp)
		{
			socket_service_component::operator=(comp);

			m_acceptor = comp.m_acceptor;
		}

		return *this;
	}

	// Utility
	void acceptor_service_component::start()
	{
		service_component::start();

		m_acceptor = http_acceptor::create(get_service());
	}

	void acceptor_service_component::stop()
	{
		// NOTE: Acceptor must be stopped BEFORE the io_service is stopped otherwise there is a thread/memory leak
		m_acceptor->close();

		service_component::stop();
	}
}

