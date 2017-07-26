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
#include "http/http_socket.h"
#include "thread/thread_safe.h"

namespace eja
{
	// Constructor
	acceptor_service_component::acceptor_service_component() : m_acceptor(http_acceptor::create(get_service())) { }
	
	acceptor_service_component::~acceptor_service_component()
	{
		thread_lock(m_sockets);
		m_sockets->clear();
	}

	// Operator
	acceptor_service_component& acceptor_service_component::operator=(const acceptor_service_component& comp)
	{
		if (this != &comp)
		{
			service_component::operator=(comp);

			m_acceptor = comp.m_acceptor;

			thread_lock(comp.m_sockets);
			m_sockets = comp.m_sockets;
		}

		return *this;
	}

	// Interface
	void acceptor_service_component::shutdown()
	{
		service_component::shutdown();

		thread_lock(m_sockets);
		m_sockets->clear();
	}

	void acceptor_service_component::clear()
	{
		//service_component::clear();

		thread_lock(m_sockets);
		m_sockets->clear();
	}

	// Utility
	void acceptor_service_component::stop()
	{
		// NOTE: Work must be reset before sockets are closed otherwise there is a thread/memory leak
		//
		set_work();

		m_acceptor->close();

		close();

		service_component::stop();

		// HACK: Fixes inability to restart
		m_acceptor = http_acceptor::create(get_service());
	}

	void acceptor_service_component::close()
	{
		// NOTE: All asio objects must be destroyed BEFORE the io_service is stopped
		// Otherwise there is a thread/memory leak
		//
		thread_lock(m_sockets);
		for (const auto& socket : m_sockets)
			socket->close();

		m_sockets->clear();
	}

	void acceptor_service_component::push(const std::shared_ptr<http_socket> socket)
	{
		assert(socket);

		thread_lock(m_sockets);
		m_sockets.insert(socket);
	}

	void acceptor_service_component::pop(const std::shared_ptr<http_socket> socket)
	{
		assert(socket);

		thread_lock(m_sockets);
		m_sockets.erase(socket);
	}

	void acceptor_service_component::on_timer(const http_socket::ptr socket, const boost::system::error_code& error)
	{
		if (error == boost::asio::error::operation_aborted)
		{
			//std::cout << "Timer Aborted: " << error.message() << std::endl;
		}
		else
		{
			socket->close();
		}
	}
}

