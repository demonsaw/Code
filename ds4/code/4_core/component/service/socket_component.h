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

#ifndef _EJA_SOCKET_COMPONENT_H_
#define _EJA_SOCKET_COMPONENT_H_

#include <memory>
#include <set>

#include "component/component.h"
#include "http/http_socket.h"
#include "thread/thread_safe.h"

namespace eja
{
	make_thread_safe_component(socket_set_component, std::set<std::shared_ptr<http_socket>>);

	class socket_component final : public component, public http_socket
	{
		make_param_factory(socket_component);

	public:
		explicit socket_component(const io_service_ptr& service) : http_socket(service) { }
		socket_component(const io_service_ptr& service, const size_t timeout) : http_socket(service, timeout) { }

		// Utility
		virtual bool valid() const override { return component::valid() && http_socket::valid(); }

		// Static		
		static ptr create(const io_service_ptr& service) { return std::make_shared<socket_component>(service); }
		static ptr create(const io_service_ptr& service, const size_t timeout) { return std::make_shared<socket_component>(service, timeout); }
	};
}

#endif
